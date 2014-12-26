//
//  Copyright (C) 2007-2014 Andrej Vodopivec <andrej.vodopivec@gmail.com>
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

#ifndef SLIDESHOWCELL_H
#define SLIDESHOWCELL_H

#include "MathCell.h"
#include <wx/image.h>

#include <wx/filesys.h>
#include <wx/fs_arc.h>

#include <vector>

using namespace std;

class SlideShow : public MathCell
{
public:
  SlideShow(wxFileSystem *filesystem = NULL);
  ~SlideShow();
  void Destroy();
  void LoadImages(wxArrayString images);
  MathCell* Copy(bool all);
  void SelectInner(wxRect& rect, MathCell** first, MathCell** last)
  {
    *first = *last = this;
  }
  int GetDisplayedIndex() { return m_displayed; }
  void SetDisplayedIndex(int ind);
  int Length() { return m_size; }
  bool ToImageFile(wxString filename);
  bool ToGif(wxString filename);
  bool CopyToClipboard();
protected:
  int m_size;
  int m_displayed;
  wxFileSystem *m_fileSystem;
  vector<wxBitmap*> m_bitmaps;
  void RecalculateSize(CellParser& parser, int fontsize, bool all);
  void RecalculateWidths(CellParser& parser, int fontsize, bool all);
  void Draw(CellParser& parser, wxPoint point, int fontsize, bool all);
  wxString ToString(bool all);
  wxString ToTeX(bool all);
	wxString ToXML(bool all);	//new!
};

#endif // SLIDESHOWCELL_H
