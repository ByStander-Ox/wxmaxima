// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//            (C) 2015 Gunter Königsmann <wxMaxima@physikbuch.de>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "XmlInspector.h"

#include <wx/sizer.h>
#include <wx/regex.h>

XmlInspector::XmlInspector(wxWindow* parent, int id) : wxTextCtrl(parent,id,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_READONLY | wxTE_RICH | wxHSCROLL | wxTE_MULTILINE)
{
  Clear();
}

XmlInspector::~XmlInspector()
{
}

void XmlInspector::Clear()
{
  wxTextCtrl::Clear();
  m_lastChar = wxChar(0);
  m_indentLevel = 0;
}

wxString XmlInspector::IndentString(int level)
{
  wxString result;
  for (int i=0;i<=level;i++)
    result += wxT(" ");
  return result;
}

void XmlInspector::Add(wxString text)
{
  int index=0;
  text.Replace(wxT("$FUNCTION:"),wxT("\n$FUNCTION:"));      
  while(index < text.Length())
  {
    wxChar ch = text[index];

    if(m_lastChar == wxT('<'))
    {
      if (ch == wxT('/'))
      {
        m_indentLevel -= 2;
      }
      else
        m_indentLevel ++;
    }

    if((m_lastChar == wxT('>')) && (ch == wxT('<')))
    {
      text = text.Left(index) + wxT ("\n") + IndentString(m_indentLevel) + text.Right(text.Length()-index);
      index += 1 + m_indentLevel; 
    }
      
    index++;
    m_lastChar = ch;
  }
  AppendText(text);
}
