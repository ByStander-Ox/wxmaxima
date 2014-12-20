///
///  Copyright (C) 2004-2014 Andrej Vodopivec <andrej.vodopivec@gmail.com>
///
///  This program is free software; you can redistribute it and/or modify
///  it under the terms of the GNU General Public License as published by
///  the Free Software Foundation; either version 2 of the License, or
///  (at your option) any later version.
///
///  This program is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///  GNU General Public License for more details.
///
///
///  You should have received a copy of the GNU General Public License
///  along with this program; if not, write to the Free Software
///  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///

#ifndef MATHCELL_H
#define MATHCELL_H

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)>(b) ? (b) : (a))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define SCALE_PX(px, scale) ((int)((double)((px)*(scale)) + 0.5))

#define MC_CELL_SKIP 0
#define MC_BASE_INDENT 12
#define MC_LINE_SKIP 2
#define MC_TEXT_PADDING 1

#define MC_GROUP_SKIP 20
#define MC_GROUP_LEFT_INDENT 15

#if defined __WXMAC__
 #define MC_EXP_INDENT 2
 #define MC_MIN_SIZE 10
 #define MC_MAX_SIZE 36
#else
 #define MC_EXP_INDENT 4
 #define MC_MIN_SIZE 8
 #define MC_MAX_SIZE 36
#endif

#include <wx_inc.h>
#include "CellParser.h"
#include "TextStyle.h"

enum {
  MC_TYPE_DEFAULT,
  MC_TYPE_MAIN_PROMPT,
  MC_TYPE_PROMPT,
  MC_TYPE_LABEL,
  MC_TYPE_INPUT,
  MC_TYPE_ERROR,
  MC_TYPE_TEXT,
  MC_TYPE_SUBSECTION,
  MC_TYPE_SECTION,
  MC_TYPE_TITLE,
  MC_TYPE_IMAGE,
  // An animation created by the with_slider_* maxima commands
  MC_TYPE_SLIDE,   
  MC_TYPE_GROUP
};

/*!
  The base class all cell types are derived from

  Besides the cell types that are directly user visible there are cells for several
  kinds of items that are displayed in a special way like abs() statements (displayed
  as horizontal rules), subscripts, superscripts and exponents.
  Another important cell type derived from the class is be the group cell that 
  groups the maxima input with the output, the input prompt and the output label.

  \attention Derived classes must test if m_next equals NULL and if it doesn't
  they have to delete() it.
 */
class MathCell
{
public:
  MathCell();
  virtual ~MathCell();
  virtual MathCell* Copy(bool all) = 0;
  virtual void Destroy() = 0;

  void AppendCell(MathCell *p_next);

  void BreakLine(bool breakLine) { m_breakLine = breakLine; }
  void BreakPage(bool breakPage) { m_breakPage = breakPage; }
  bool BreakLineHere();
  bool ForceBreakLineHere() { return m_forceBreakLine; }
  bool BreakPageHere() { return m_breakPage; }
  virtual bool BreakUp() { return false; }

  bool ContainsRect(wxRect& big, bool all = true);
  bool ContainsPoint(wxPoint& point)
  {
    return GetRect().Contains(point);
  }
  void CopyData(MathCell *s, MathCell *t);

  virtual void Draw(CellParser& parser, wxPoint point, int fontsize, bool all);
  void DrawBoundingBox(wxDC& dc, bool all = false, int border = 0);
  bool DrawThisCell(CellParser& parser, wxPoint point);

  void ForceBreakLine(bool force) { m_forceBreakLine = m_breakLine = force; }

  int GetHeight() { return m_height; }
  int GetWidth() { return m_width; }
  int GetCenter() { return m_center; }
  int GetDrop() { return m_height - m_center; }
  int GetType() { return m_type; }
  int GetMaxDrop();
  int GetMaxCenter();
  int GetMaxHeight();
  int GetFullWidth(double scale);
  int GetLineWidth(double scale);
  int GetCurrentX() { return m_currentPoint.x; }
  int GetCurrentY() { return m_currentPoint.y; }
  virtual wxRect GetRect(bool all = false);
  virtual wxString GetDiffPart();

  virtual void RecalculateSize(CellParser& parser, int fontsize, bool all);
  virtual void RecalculateWidths(CellParser& parser, int fontsize, bool all);
  void ResetData();
  void ResetSize() { m_width = m_height = -1; }

  void SetSkip(bool skip) { m_bigSkip = skip; }
  void SetType(int type);
  int GetStyle(){ return m_textStyle; }	//l'ho aggiunto io

  void SetPen(CellParser& parser);
  void SetHighlight(bool highlight) { m_highlight = highlight; }
  virtual void SetExponentFlag() { }
  virtual void SetValue(wxString text) { }
  virtual wxString GetValue() { return wxEmptyString; }

  void SelectRect(wxRect& rect, MathCell** first, MathCell** last);
  void SelectFirst(wxRect& rect, MathCell** first);
  void SelectLast(wxRect& rect, MathCell** last);
  virtual void SelectInner(wxRect& rect, MathCell** first, MathCell** last);

  virtual bool IsOperator();
  bool IsCompound();
  virtual bool IsShortNum() { return false; }

  MathCell* GetParent();

  virtual wxString ToString(bool all);
  virtual wxString ToTeX(bool all);
	virtual wxString ToXML(bool all);

  void UnsetPen(CellParser& parser);
  virtual void Unbreak(bool all);

  MathCell *m_next, *m_previous, *m_group;
  MathCell *m_nextToDraw, *m_previousToDraw;
  wxPoint m_currentPoint;  // Current point in console (the center of the cell)
  bool m_bigSkip;
  bool m_isBroken;
  bool m_isHidden;
  bool IsComment()
  {
    return m_type == MC_TYPE_TEXT || m_type == MC_TYPE_SECTION ||
           m_type == MC_TYPE_SUBSECTION || m_type == MC_TYPE_TITLE;
  }
  bool IsEditable(bool input = false)
  {
    return (m_type == MC_TYPE_INPUT  &&
            m_previous != NULL && m_previous->m_type == MC_TYPE_MAIN_PROMPT)
         || (!input && IsComment());
  }
  virtual void ProcessEvent(wxKeyEvent& event) { }
  virtual bool ActivateCell() { return false; }
  virtual bool AddEnding() { return false; }
  virtual void SelectPointText(wxDC &dc, wxPoint& point) { }
  virtual void SelectRectText(wxDC &dc, wxPoint& one, wxPoint& two) { }
  virtual void PasteFromClipboard(bool primary = false) { }
  virtual bool CopyToClipboard() { return false; }
  virtual bool CutToClipboard() { return false; }
  virtual void SelectAll() { }
  virtual bool CanCopy() { return false; }
  virtual void SetMatchParens(bool match) { }
  virtual wxPoint PositionToPoint(CellParser& parser, int pos = -1) { return wxPoint(-1, -1); }
  virtual bool IsDirty() { return false; }
  virtual void SwitchCaretDisplay() { }
  virtual void SetFocus(bool focus) { }
  void SetForeground(CellParser& parser);
  virtual bool IsActive() { return false; }
  virtual void SetParent(MathCell *parent, bool all);
  void SetStyle(int style) { m_textStyle = style; }
  bool IsMath();
  void SetAltCopyText(wxString text) { m_altCopyText = text; }
protected:
  int m_height;
  int m_width;
  int m_fullWidth;
  int m_lineWidth;
  int m_center;
  int m_maxCenter;
  int m_maxDrop;
  int m_type;
  int m_textStyle;
  bool m_breakPage;
  bool m_breakLine;
  bool m_forceBreakLine;
  bool m_highlight;
  wxString m_altCopyText; // m_altCopyText is not check in all cells!
};

#endif // MATHCELL_H
