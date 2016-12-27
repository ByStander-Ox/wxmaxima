// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2009-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2015 Gunter Königsmann <wxMaxima@physikbuch.de>
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

  This file contains the definition of the class TableOfContents that handles the 
  table of contents pane.
 */
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "GroupCell.h"
#include "EditorCell.h"

#ifndef TABLEOFCONTENTS_H
#define TABLEOFCONTENTS_H

enum {
  structure_ctrl_id = 4,
  structure_regex_id
};

/*! This class generates a pane containing the table of contents.

 */
class TableOfContents : public wxPanel
{
public:
  enum PopIds{
    /*! The "copy" popup menu item was clicked

      This item is the first of the enum and is assigned a high enough number
      that it won't collide with the numbers to be found in wxFrame::Event
     */
    popid_Fold = wxID_HIGHEST + 500,
    popid_Unfold = wxID_HIGHEST + 501
  };
  TableOfContents(wxWindow* parent, int id);
  /* The destructor
   */
  ~TableOfContents();
  void OnMouseRightDown(wxListEvent& event);
  //! Add a file to the recently opened files list.
  void AddToTableOfContents(wxString cmd);
  //! What happens if someone changes the search box contents
  void OnRegExEvent(wxCommandEvent &ev);
  /*! Update the structure information from the tree 

    Since this function traverses the tree and we don't want it 
    to impact the performance too much
      - we call it only on creation of a cell and on leaving it again
      - and we only traverse the tree if the pane is actually shown.
   */
  void Update(GroupCell* tree,GroupCell *pos);
  //! Get the nth Cell in the table of contents.
  GroupCell *GetCell(int index);

  //! Returns the cell that was last right-clicked on.
  GroupCell *RightClickedOn(){return m_cellRightClickedOn;}
protected:
  void OnSize(wxSizeEvent& event);
private:
  GroupCell *m_cellRightClickedOn;
  //! The last selected item
  long m_lastSelection;
  //! Update the displayed contents.
  void UpdateDisplay();
  wxListCtrl *m_displayedItems;
  wxTextCtrl *m_regex;
  //! The items we displayed the last time update() was called
  wxArrayString m_items_old;
  
  std::vector <GroupCell *> m_structure;
  DECLARE_EVENT_TABLE()
};

#endif // TABLEOFCONTENTS_H
