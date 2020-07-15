// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//  Copyright (C) 2014-2018 Gunter Königsmann <wxMaxima@physikbuch.de>
//  Copyright (C) 2020      Kuba Ober <kuba@bertec.com>
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

#include "CellPointers.h"
#include <algorithm>

CellPointers::CellPointers(wxScrolledCanvas *worksheet) :
  m_worksheet(worksheet)
{}

wxString CellPointers::WXMXGetNewFileName()
{
  wxString file(wxT("image"));
  file << (++m_wxmxImgCounter) << wxT(".");
  return file;
}

void CellPointers::ErrorList::Remove(GroupCell * cell)
{
  m_errors.erase(std::remove(m_errors.begin(), m_errors.end(), cell), m_errors.end());
}

bool CellPointers::ErrorList::Contains(GroupCell * cell) const
{
  return std::find(m_errors.begin(), m_errors.end(), cell) != m_errors.end();
}

void CellPointers::ErrorList::Add(GroupCell * cell)
{ m_errors.emplace_back(cell); }

void CellPointers::SetWorkingGroup(GroupCell *group)
{
  if (group)
    m_lastWorkingGroup = group;
  m_workingGroup = group;
}

GroupCell *CellPointers::GetWorkingGroup(bool resortToLast) const
{ return (m_workingGroup || !resortToLast) ? m_workingGroup : m_lastWorkingGroup; }

GroupCell *CellPointers::ErrorList::FirstError() const
{ return m_errors.empty() ? nullptr : m_errors.front().get(); }

GroupCell *CellPointers::ErrorList::LastError() const
{ return m_errors.empty() ? nullptr : m_errors.back().get(); }
