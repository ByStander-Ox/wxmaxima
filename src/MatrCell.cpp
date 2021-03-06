// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2018 Gunter Königsmann <wxMaxima@physikbuch.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//  SPDX-License-Identifier: GPL-2.0+

/*! \file
  This file defines the class MatrCell

  MatrCell is the Cell type that represents matrices and matrix-like 
  elements like the table_form command.
*/

#include "MatrCell.h"

MatrCell::MatrCell(GroupCell *parent, Configuration **config) :
    Cell(parent, config)
{
  InitBitFields();
}

MatrCell::MatrCell(const MatrCell &cell):
    MatrCell(cell.m_group, cell.m_configuration)
{
  CopyCommonData(cell);
  m_specialMatrix = cell.m_specialMatrix;
  m_inferenceMatrix = cell.m_inferenceMatrix;
  m_roundedParens = cell.m_roundedParens;
  m_rowNames = cell.m_rowNames;
  m_colNames = cell.m_colNames;
  m_matWidth = cell.m_matWidth;
  m_matHeight = cell.m_matHeight;
  for (unsigned int i = 0; i < cell.m_matWidth * cell.m_matHeight; i++)
    if (i < cell.m_cells.size())
      m_cells.emplace_back(cell.m_cells[i]->CopyList());
}

std::unique_ptr<Cell> MatrCell::Copy() const
{
  return std::make_unique<MatrCell>(*this);
}

void MatrCell::Recalculate(AFontSize const fontsize)
{
  if(!NeedsRecalculation(fontsize))
    return;

  AFontSize const fontsize_entry{ MC_MIN_SIZE, fontsize - 2 };
  for (unsigned int i = 0; i < m_cells.size(); i++)
    m_cells[i]->RecalculateList(fontsize_entry);

  m_width = 0;
  m_widths.clear();
  for (unsigned int i = 0; i < m_matWidth; i++)
  {
    int width = 0;
    for (unsigned int j = 0; j < m_matHeight; j++)
    {
      if((m_matWidth * j + i)<m_cells.size())
        width = wxMax(width, m_cells[m_matWidth * j + i]->GetFullWidth());
    }
    m_widths.emplace_back(width);
    m_width += (width + Scale_Px(10));
  }
  if (m_width < Scale_Px(14))
    m_width = Scale_Px(14);

  m_height = 0;
  m_dropCenters.clear();
  for (unsigned int i = 0; i < m_matHeight; i++)
  {
    int center = 0, drop = 0;
    for (unsigned int j = 0; j < m_matWidth; j++)
      if(m_matWidth * i + j < m_cells.size())
      {
        center = wxMax(center, m_cells[m_matWidth * i + j]->GetCenterList());
        drop = wxMax(drop, m_cells[m_matWidth * i + j]->GetMaxDrop());
      }
    m_dropCenters.emplace_back(drop, center);
    m_height += (center + drop + Scale_Px(10));
  }
  if (m_height == 0)
    m_height = fontsize + Scale_Px(10);
  m_center = m_height / 2;

  Cell::Recalculate(fontsize);
}

void MatrCell::Draw(wxPoint point)
{
  Cell::Draw(point);
  if (DrawThisCell(point))
  {
    Configuration *configuration = (*m_configuration);
    wxDC *dc = configuration->GetDC();
    wxPoint mp;
    mp.x = point.x + Scale_Px(5);
    mp.y = point.y - m_center;
    for (unsigned int i = 0; i < m_matWidth; i++)
    {
      mp.y = point.y - m_center + Scale_Px(5);
      for (unsigned int j = 0; j < m_matHeight; j++)
      {
        if((j * m_matWidth + i) < m_cells.size())
          {
            mp.y += m_dropCenters[j].center;
            wxPoint mp1(mp);
            mp1.x = mp.x + (m_widths[i] - m_cells[j * m_matWidth + i]->GetFullWidth()) / 2;
            m_cells[j * m_matWidth + i]->DrawList(mp1);
            mp.y += (m_dropCenters[j].drop + Scale_Px(10));
          }
      }
      mp.x += (m_widths[i] + Scale_Px(10));
    }
    SetPen(1.5);
    if (m_specialMatrix)
    {
      if (m_inferenceMatrix)
        dc->DrawLine(point.x + Scale_Px(1),
                    point.y - m_center + Scale_Px(2),
                    point.x + Scale_Px(1),
                    point.y + m_center - Scale_Px(2));
      else
      {
        if (m_rowNames)
          dc->DrawLine(point.x + m_widths[0] + 2 * Scale_Px(5),
                      point.y - m_center + Scale_Px(2),
                      point.x + m_widths[0] + 2 * Scale_Px(5),
                      point.y + m_center - Scale_Px(2));
        if (m_colNames)
          dc->DrawLine(point.x + Scale_Px(1),
                      point.y - m_center + m_dropCenters[0].Sum() + 2 * Scale_Px(5),
                      point.x + Scale_Px(1) + m_width,
                      point.y - m_center + m_dropCenters[0].Sum() + 2 * Scale_Px(5));
      }
    }
    else
    {
      wxDC &adc = *configuration->GetAntialiassingDC();
      if(m_roundedParens)
      {
        SetPen(1);
        int signWidth = Scale_Px(4);
        if (m_height <= signWidth / 3)
          signWidth = m_height / 3;
        
        // Left bracket
        wxPoint pointsL[5] = {
          {point.x + Scale_Px(1) + signWidth,
           point.y - m_center},
          {point.x + Scale_Px(1) + signWidth / 2,
           point.y - m_center + signWidth / 2},
          {point.x + Scale_Px(1),
           point.y},
          {point.x + Scale_Px(1) + signWidth / 2,
           point.y + m_center - signWidth / 2},
          {point.x + Scale_Px(1) + signWidth,
           point.y + m_center}
        };
        adc.DrawSpline(5, pointsL);
        pointsL[2] = {point.x + Scale_Px(1.5),
                      point.y};
        adc.DrawSpline(5, pointsL);

        // Right bracket
        wxPoint pointsR[5] = {
          {point.x + m_width - Scale_Px(1) - signWidth,
           point.y - m_center},
          {point.x + m_width - Scale_Px(1) - signWidth / 2,
           point.y - m_center + signWidth / 2},
          {point.x + m_width - Scale_Px(1.5),
           point.y},
          {point.x + m_width - Scale_Px(1) - signWidth / 2,
           point.y + m_center - signWidth / 2},
          {point.x + m_width - Scale_Px(1) - signWidth,
           point.y + m_center}
        };
        adc.DrawSpline(5, pointsR);
        pointsR[2] = {point.x + m_width - Scale_Px(1),
                      point.y};
        adc.DrawSpline(5, pointsR);
      }
      else
      {
        SetPen(1.5);
        // left bracket
        const wxPoint pointsL[4] = {
          {Scale_Px(5),
           -m_center + Scale_Px(2)},
          {Scale_Px(1),
           -m_center + Scale_Px(2)},
          {Scale_Px(1),
           m_center - Scale_Px(2)},
          {Scale_Px(5),
           m_center - Scale_Px(2)}
        };
        adc.DrawLines(4, pointsL, point.x, point.y);

        // right bracket
        const wxPoint pointsR[4] = {
          {-Scale_Px(5),
           -m_center + Scale_Px(2)},
          {-Scale_Px(1),
           -m_center + Scale_Px(2)},
          {-Scale_Px(1),
           m_center - Scale_Px(2)},
          {-Scale_Px(5),
           m_center - Scale_Px(2)}
        };
        adc.DrawLines(4, pointsR, point.x + m_width - 1, point.y);
      }
    }
  }
}

void MatrCell::AddNewCell(std::unique_ptr<Cell> &&cell)
{
  m_cells.emplace_back(std::move(cell));
}

wxString MatrCell::ToString() const
{
  wxString s = wxT("matrix(\n");
  for (unsigned int i = 0; i < m_matHeight; i++)
  {
    s += wxT("\t\t[");
    for (unsigned int j = 0; j < m_matWidth; j++)
    {
	  s += m_cells[i * m_matWidth + j]->ListToString();
      if (j < m_matWidth - 1)
        s += wxT(",\t");
    }
    s += wxT("]");
    if (i < m_matHeight - 1)
      s += wxT(",");
    s += wxT("\n");
  }
  s += wxT("\t)");
  return s;
}

wxString MatrCell::ToMatlab() const
{
	//ToDo: We ignore colNames and rowNames here. Are they currently in use?
	wxString s;

	s = wxT("[");
	for (unsigned int i = 0; i < m_matHeight; i++)
	{
	  for (unsigned int j = 0; j < m_matWidth; j++)
	  {
		s += m_cells[i * m_matWidth + j]->ListToMatlab();
		if (j < m_matWidth - 1)
		  s += wxT(", ");
	  }
	  if (i < m_matHeight - 1)
		s += wxT(";\n");
	}

	s += wxT("];");

	return s;
}

wxString MatrCell::ToTeX() const
{
  //ToDo: We ignore colNames and rowNames here. Are they currently in use?
  wxString s;

  if (!m_specialMatrix)
  {
    if(m_roundedParens)
      s = wxT("\\begin{pmatrix}");
    else
      s = wxT("\\begin{bmatrix}");
  }
  else
  {
    s = wxT("\\begin{array}{");
    for (unsigned int j = 0; j < m_matWidth; j++)
      s += wxT("c");
    s += wxT("}");
  }
  for (unsigned int i = 0; i < m_matHeight; i++)
  {
    for (unsigned int j = 0; j < m_matWidth; j++)
    {
      s += m_cells[i * m_matWidth + j]->ListToTeX();
      if (j < m_matWidth - 1)
        s += wxT(" & ");
    }
    if (i < m_matHeight - 1)
      s += wxT("\\\\\n");
  }
  if (!m_specialMatrix)
  {
    if(m_roundedParens)
      s += wxT("\\end{pmatrix}");
    else
      s += wxT("\\end{bmatrix}");
  }
  else
    s += wxT("\\end{array}");
  return s;
}

wxString MatrCell::ToMathML() const
{
  wxString retval;
  if (!m_specialMatrix)
    retval = wxT("<mrow><mo>(</mo><mrow>");
  retval += wxT("<mtable>");

  for (unsigned int i = 0; i < m_matHeight; i++)
  {
    retval += wxT("<mtr>");
    for (unsigned int j = 0; j < m_matWidth; j++)
      retval += wxT("<mtd>") + m_cells[i * m_matWidth + j]->ListToMathML() + wxT("</mtd>");
    retval += wxT("</mtr>");
  }
  retval += wxT("</mtable>\n");
  if (!m_specialMatrix)
    retval += wxT("</mrow><mo>)</mo></mrow>\n");
  return retval;
}

wxString MatrCell::ToOMML() const
{
  wxString retval;

  retval = wxT("<m:d>");
  if (!m_specialMatrix)
  {
    if(m_roundedParens)
      retval += wxT("<m:dPr><m:begChr>(</m:begChr><m:endChr>)</m:endChr> <m:grow>\"1\"</m:grow></m:dPr>");
    else
      retval += wxT("<m:dPr><m:begChr>[</m:begChr><m:endChr>]</m:endChr> <m:grow>\"1\"</m:grow></m:dPr>");
  }
  
  retval += wxT("<m:e><m:m>");

  for (unsigned int i = 0; i < m_matHeight; i++)
  {
    retval += wxT("<m:mr>");
    for (unsigned int j = 0; j < m_matWidth; j++)
      retval += wxT("<m:e>") + m_cells[i * m_matWidth + j]->ListToOMML() + wxT("</m:e>");
    retval += wxT("</m:mr>");
  }

  retval += wxT("</m:m></m:e></m:d>");
  return retval;
}

wxString MatrCell::ToXML() const
{
  wxString flags;
  if (m_forceBreakLine)
    flags += wxT(" breakline=\"true\"");
  if (m_roundedParens)
    flags += wxT(" roundedParens=\"true\"");

  wxString s = wxEmptyString;
  if (m_specialMatrix)
    s = wxString::Format(
      wxT("<tb") + flags + wxT(" special=\"true\" inference=\"%s\" rownames=\"%s\" colnames=\"%s\">"),
            m_inferenceMatrix ? "true" : "false",
            m_rowNames ? "true" : "false",
            m_colNames ? "true" : "false");
  else
    s = wxT("<tb") +flags +wxT(">");

  for (unsigned int i = 0; i < m_matHeight; i++)
  {
    s += wxT("<mtr>");
    for (unsigned int j = 0; j < m_matWidth; j++)
      s += wxT("<mtd>") + m_cells[i * m_matWidth + j]->ListToXML() + wxT("</mtd>");
    s += wxT("</mtr>");
  }
  s += wxT("</tb>");

  return s;
}

void MatrCell::SetDimension()
{
  if (m_matHeight != 0)
    m_matWidth = m_matWidth / m_matHeight;
}
