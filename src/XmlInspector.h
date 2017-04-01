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

/*! \file

  This file contains the definition of the class XmlInspector that handles the 
  table of contents pane.
 */
#include <wx/wx.h>
#include <vector>
#include "GroupCell.h"

#ifndef XMLINSPECTOR_H
#define XMLINSPECTOR_H

enum
{
  XmlInspector_ctrl_id = 4,
  XmlInspector_regex_id
};

/*! This class generates a pane containing the table of contents.

 */
class XmlInspector : public wxTextCtrl
{
public:
  XmlInspector(wxWindow *parent, int id);

  /*! The destructor
   */
  ~XmlInspector();

  //! Remove all text from the editor.
  virtual void Clear();

  //! Add some text.
  void Add(wxString text);

private:
  wxChar m_lastChar;
  int m_indentLevel;

  wxString IndentString(int level);
};

#endif // XMLINSPECTOR_H
