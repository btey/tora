/****************************************************************************
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000 GlobeCom AB
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *      As a special exception, you have permission to link this program
 *      with the Qt and Oracle Client libraries and distribute executables,
 *      as long as you follow the requirements of the GNU GPL in regard to
 *      all of the software in the executable aside from Qt and Oracle client
 *      libraries.
 *
 ****************************************************************************/

TO_NAMESPACE;

#include <stdio.h>
#include <ctype.h>

#include <qclipboard.h>
#include <qheader.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qpaintdevicemetrics.h>
#include <qdragobject.h>

#include "toresultview.h"
#include "toresultview.moc"
#include "tomain.h"
#include "tosql.h"
#include "toconf.h"
#include "totool.h"
#include "tomemoeditor.h"

static int MaxColDisp;

void toResultViewMLine::setText (int col,const QString &text)
{
  QListViewItem::setText(col,text);
  int pos=0;
  int lines=0;
  do {
    pos=text.find("\n",pos);
    lines++;
    pos++;
  } while(pos>0);
  if (lines>Lines)
    Lines=lines;
}

void toResultViewMLine::setup(void)
{
  QListViewItem::setup();
  int margin=listView()->itemMargin()*2+1;
  setHeight((height()-margin)*Lines+margin);
}

void toResultViewMLine::paintCell (QPainter *pnt,const QColorGroup & cg,
				   int column,int width,int alignment)
{
  toResultViewItem::paintCell(pnt,cg,column,
			      max(QListViewItem::width(pnt->fontMetrics(),listView(),column),width),
			      alignment);
}

static int TextWidth(const QFontMetrics &fm,const QString &str)
{
  int lpos=0;
  int pos=0;
  int maxWidth=0;
  const char *strtext=str;
  do {
    pos=str.find("\n",lpos);
    QRect bounds=fm.boundingRect(strtext+lpos,pos-lpos);
    if (bounds.width()>maxWidth)
      maxWidth=bounds.width();
    lpos=pos+1;
  } while(pos>=0);
  return maxWidth;
}

int toResultViewMLine::width(const QFontMetrics &fm, const QListView *top, int column) const
{
  if (!MaxColDisp)
    MaxColDisp=toTool::globalConfig(CONF_MAX_COL_DISP,DEFAULT_MAX_COL_DISP).toInt();
  return min(TextWidth(fm,text(column)),MaxColDisp)+top->itemMargin()*2+2;
}

int toResultViewItem::width(const QFontMetrics &fm, const QListView *top, int column) const
{
  if (!MaxColDisp)
    MaxColDisp=toTool::globalConfig(CONF_MAX_COL_DISP,DEFAULT_MAX_COL_DISP).toInt();
  QRect bounds=fm.boundingRect(text(column));
  return min(bounds.width(),MaxColDisp)+top->itemMargin()*2+2;
}

void toResultViewItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
  QListViewItem::paintCell(p,cg,column,width,align);
  if (itemBelow()==NULL||itemBelow()->itemBelow()==NULL) {
    toResultView *view=(toResultView *)listView();
    view->addItem();
  }
}

QString toResultViewItem::text(int col) const
{
  QString text=QListViewItem::text(col);
  int pos=text.find('\n');
  if (pos!=-1) {
    text.remove(pos,text.length());
    text.append("...");
  }
  
  return text;
}

QString toResultViewItem::key(int col,bool asc) const
{
  static QRegExp number("^\\d*\\.?\\d+$");
  QString val=text(col);
  if (number.match(val)>=0) {
    static char buf[100];
    sprintf(buf,"%015f",text(col).toFloat());
    return buf;
  }
  return val;
}

void toResultViewMLCheck::setText (int col,const QString &text)
{
  QCheckListItem::setText(col,text);
  int pos=0;
  int lines=0;
  do {
    pos=text.find("\n",pos);
    lines++;
    pos++;
  } while(pos>0);
  if (lines>Lines)
    Lines=lines;
}

void toResultViewMLCheck::setup(void)
{
  QCheckListItem::setup();
  int margin=listView()->itemMargin()*2+1;
  setHeight((height()-margin)*Lines+margin);
}

void toResultViewMLCheck::paintCell (QPainter *pnt,const QColorGroup & cg,
				   int column,int width,int alignment)
{
  toResultViewCheck::paintCell(pnt,cg,column,
			      max(QCheckListItem::width(pnt->fontMetrics(),listView(),column),width),
			      alignment);
}

int toResultViewMLCheck::width(const QFontMetrics &fm, const QListView *top, int column) const
{
  if (!MaxColDisp)
    MaxColDisp=toTool::globalConfig(CONF_MAX_COL_DISP,DEFAULT_MAX_COL_DISP).toInt();
  return min(TextWidth(fm,text(column)),MaxColDisp)+top->itemMargin()*2+2;
}

int toResultViewCheck::width(const QFontMetrics &fm, const QListView *top, int column) const
{
  if (!MaxColDisp)
    MaxColDisp=toTool::globalConfig(CONF_MAX_COL_DISP,DEFAULT_MAX_COL_DISP).toInt();
  QRect bounds=fm.boundingRect(text(column));
  return min(bounds.width(),MaxColDisp)+top->itemMargin()*2+2;
}


void toResultViewCheck::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
  QCheckListItem::paintCell(p,cg,column,width,align);
  if (itemBelow()==NULL||itemBelow()->itemBelow()==NULL) {
    toResultView *view=(toResultView *)listView();
    view->addItem();
  }
}

QString toResultViewCheck::text(int col) const
{
  QString text=QCheckListItem::text(col);
  int pos=text.find('\n');
  if (pos!=-1) {
    text.remove(pos,text.length());
    text.append("...");
  }
  
  return text;
}

QString toResultViewCheck::key(int col,bool asc) const
{
  static QRegExp number("^\\d*\\.?\\d+$");
  QString val=text(col);
  if (number.match(val)>=0) {
    static char buf[100];
    sprintf(buf,"%015f",text(col).toFloat());
    return buf;
  }
  return val;
}

class toResultTip : public QToolTip {
private:
  toResultView *Result;
public:
  toResultTip(toResultView *parent)
    : QToolTip(parent->viewport())
  {
    Result=parent;
  }

  virtual void maybeTip(const QPoint &p)
  {
    QListViewItem *item=Result->itemAt(p);
    toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(item);
    toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(item);
    if (item) {
      int col=Result->header()->sectionAt(Result->viewportToContents(p).x());
      QPoint t(Result->header()->sectionPos(col),0);
      int pos=Result->contentsToViewport(t).x();
      int width=Result->header()->sectionSize(col);
      QString key;
	
      QString text=item->text(col);
      if (resItem)
	key=resItem->tooltip(col);
      else if (chkItem)
	key=chkItem->tooltip(col);
      else
	key=text;
      int textWidth=TextWidth(qApp->fontMetrics(),text)+Result->itemMargin()*2+2;
      if (key!=text||
	  width<textWidth) {
	QRect itemRect=Result->itemRect(item);
	itemRect.setLeft(pos);
	itemRect.setRight(pos+width);
	tip(itemRect,key);
      }
    }
  }
};

void toResultView::setup(bool readable,bool dispCol)
{
  Query=NULL;
  ReadableColumns=readable;
  setAllColumnsShowFocus(true);
  NumberColumn=dispCol;
  if (NumberColumn)
    addColumn("#");
  AllResult=new toResultTip(this);
  setShowSortIndicator(true);
  setSorting(-1);
  Menu=NULL;
  connect(this,SIGNAL(rightButtonPressed(QListViewItem *,const QPoint &,int)),
	  this,SLOT(displayMenu(QListViewItem *,const QPoint &,int)));
}

toResultView::toResultView(bool readable,bool dispCol,toConnection &conn,QWidget *parent,const char *name)
  : QListView(parent,name), Connection(conn)
{
  setup(readable,dispCol);
}

toResultView::toResultView(toConnection &conn,QWidget *parent,const char *name)
  : QListView(parent,name), Connection(conn)
{
  setup(false,true);
}

void toResultView::addItem(void)
{
  int MaxColSize=toTool::globalConfig(CONF_MAX_COL_SIZE,DEFAULT_MAX_COL_SIZE).toInt();
  MaxColDisp=toTool::globalConfig(CONF_MAX_COL_DISP,DEFAULT_MAX_COL_DISP).toInt();

  try {
    if (Query&&!Query->eof()) {
      RowNumber++;
      int disp=0;
      LastItem=createItem(this,LastItem,NULL);
      if (NumberColumn) {
	LastItem->setText(0,QString::number(RowNumber));
	disp=1;
      } else
	LastItem->setText(columns(),QString::number(RowNumber));
      for (int j=0;(j<DescriptionLen||j==0)&&!Query->eof();j++)
	LastItem->setText(j+disp,toReadValue(Description[j],*Query,MaxColSize));
    }
  } TOCATCH
}

void toResultView::contentsMouseDoubleClickEvent (QMouseEvent *e)
{
  QPoint p=e->pos();
  int col=header()->sectionAt(p.x());
  QListViewItem *item=itemAt(contentsToViewport(p));
  toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(item);
  toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(item);
  QClipboard *clip=qApp->clipboard();
  if (resItem)
    clip->setText(resItem->allText(col));
  else if (chkItem)
    clip->setText(chkItem->allText(col));
  else if (item)
    clip->setText(item->text(col));
  
}

void toResultView::contentsMouseMoveEvent (QMouseEvent *e)
{
  if (e->state()==LeftButton&&
      e->stateAfter()==LeftButton) {
    QPoint p=e->pos();
    int col=header()->sectionAt(p.x());
    QListViewItem *item=itemAt(contentsToViewport(p));
    toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(item);
    toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(item);
    QString str;
    if (resItem)
      str=resItem->allText(col);
    else if (chkItem)
      str=chkItem->allText(col);
    else if (item)
      str=item->text(col);
    if (str.length()) {
      QDragObject *d=new QTextDrag(str,this);
      d->dragCopy();
    }
  } else
    QListView::contentsMouseMoveEvent(e);
}

void toResultView::query(const QString &sql,const list<QString> &param)
{
  delete Query;
  SQL=sql;
  Query=NULL;
  LastItem=NULL;
  RowNumber=0;

  clear();
  while(columns()>1) {
    removeColumn(1);
  }
  if (!NumberColumn)
    removeColumn(0);

  try {
    Query=new otl_stream;
    Query->set_all_column_types(otl_all_num2str|otl_all_date2str);
    Query->open(1,
		sql.utf8(),
		Connection.connection());

    {
      otl_null null;
      for (list<QString>::iterator i=((list<QString> &)param).begin();i!=((list<QString> &)param).end();i++) {
	if ((*i).isNull())
	  (*Query)<<null;
        else
    	  (*Query)<<(*i).utf8();
      }
    }

    Description=Query->describe_select(DescriptionLen);

    bool hidden=false;

    for (int i=0;i<DescriptionLen;i++) {
      QString name(QString::fromUtf8(Description[i].name));
      if (ReadableColumns) {
	bool inWord=false;
	for (unsigned int j=0;j<name.length();j++) {
	  if (name.at(j)=='_')
	    name.ref(j)=' ';
	  if (name.at(j).isSpace())
	    inWord=false;
	  else if (name.at(j).isLetter()) {
	    if (inWord)
	      name.ref(j)=name.at(j).lower();
	    else
	      name.ref(j)=name.at(j).upper();
	    inWord=true;
	  }
	}
      }
      if (name.length()>0&&name.at(0)!=' ') {
	if (hidden)
	  throw QString("Can only hide last column in query");
	if (name.at(0)=='-') {
	  addColumn(name.right(name.length()-1));
	  setColumnAlignment(columns()-1,AlignRight);
	} else
	  addColumn(name);
      } else
	hidden=true;
    }

    if (NumberColumn)
      setSorting(0);
    else
      setSorting(DescriptionLen);

    int MaxNumber=toTool::globalConfig(CONF_MAX_NUMBER,DEFAULT_MAX_NUMBER).toInt();
    for (int j=0;j<MaxNumber&&!Query->eof();j++)
      addItem();
    if (ReadAll)
      readAll();

    char buffer[100];
    if (Query->get_rpc()>0)
      sprintf(buffer,"%d rows processed",(int)Query->get_rpc());
    else
      sprintf(buffer,"Query executed");
    toStatusMessage(buffer);
  } TOCATCH
  updateContents();
}

void toResultView::readAll(void)
{
  toStatusMessage("Reading all entries");
  int i=0;
  while(!eof()) {
    addItem();
    i++;
    if (i%100==0)
      qApp->processEvents();
  }
}

toResultView::~toResultView()
{
  delete Query;
  if (qApp->focusWidget()==this)
    toMain::editDisable();
}

QListViewItem *toResultView::printPage(QPrinter *printer,QPainter *painter,QListViewItem *top,int &column,int &level,int pageNo,bool paint)
{
  QPaintDeviceMetrics metrics(printer);
  double scale=toTool::globalConfig(CONF_LIST_SCALE,DEFAULT_LIST_SCALE).toFloat();
  double mwidth=metrics.width()/scale;
  double mheight=metrics.height()/scale;
  double x=0;
  if (paint) {
    QString numPage("Page: ");
    numPage+=QString::number(pageNo);
    painter->drawText(0,metrics.height()-header()->height(),metrics.width(),
		      header()->height(),
		      SingleLine|AlignRight|AlignVCenter,
		      numPage);
    painter->drawText(0,metrics.height()-header()->height(),metrics.width(),
		      header()->height(),
		      SingleLine|AlignHCenter|AlignVCenter,
		      Connection.connectString());
    painter->drawText(0,metrics.height()-header()->height(),metrics.width(),
		      header()->height(),
		      SingleLine|AlignLeft|AlignVCenter,
		      sqlName());
    painter->scale(scale,scale);
    painter->drawLine(0,header()->height()-1,int(mwidth),header()->height()-1);
  }
  for (int i=column;i<columns();i++) {
    double width=columnWidth(i);
    if (width+x>=mwidth) {
      if (i==column)
	width=mwidth-x-1;
      else
	break;
    }
    if (paint)
      painter->drawText(int(x),0,int(width),header()->height(),SingleLine|AlignLeft|AlignVCenter,header()->label(i));
    x+=width;
  }
  if (paint)
    painter->translate(0,header()->height());

  double y=(header()->height()+1)/scale+header()->height();
  int curLevel=level;
  int tree=rootIsDecorated()?treeStepSize():0;
  int newCol=-1;
  QListViewItem *item=top;
  while(item&&(y<mheight||item==top)) {
    if (column==0)
      x=curLevel;
    else
      x=0;
    painter->translate(x,0);
    for (int i=column;i<columns();i++) {
      double width=columnWidth(i);
      if (width+x>=mwidth) {
	if (i==column)
	  width=mwidth-x-1;
	else {
	  newCol=i;
	  break;
	}
      }
      if (i==0)
	width-=curLevel;
      if (paint) {
	item->paintCell(painter,qApp->palette().active(),i,int(width),columnAlignment(i));
	painter->translate(width,0);
      }
      x+=width;
    }
    if (paint)
      painter->translate(-x,item->height());
    y+=item->height();
    if (item->firstChild()) {
      item=item->firstChild();
      curLevel+=tree;
    } else if (item->nextSibling())
      item=item->nextSibling();
    else {
      do {
	item=item->parent();
	curLevel-=tree;
      } while(item&&!item->nextSibling());
      if (item)
	item=item->nextSibling();
    }
  }
  if (paint)
    painter->drawLine(0,0,int(mwidth),0);
  if (newCol>=0) {
    column=newCol;
    return top;
  }
  column=0;
  level=curLevel;
  return item;
}

void toResultView::print(void)
{
  readAll();
  TOPrinter printer;
  printer.setMinMax(1,1000);
  if (printer.setup()) {
    printer.setCreator("TOra");
    QPainter painter(&printer);
    QListViewItem *item=firstChild();
    int column=0;
    int tree=rootIsDecorated()?treeStepSize():0;
    int page=1;
    while(page<printer.fromPage()&&
	  (item=printPage(&printer,&painter,item,column,tree,page++,false)))
      painter.resetXForm();
    while((item=printPage(&printer,&painter,item,column,tree,page++))&&
	  (printer.toPage()==0||page<=printer.toPage())) {
      printer.newPage();
      painter.resetXForm();
      qApp->processEvents();
      QString str("Printing page ");
      str+=QString::number(page);
      toStatusMessage(str);
    }
    painter.end();
    toStatusMessage("Done printing");
  }
}

void toResultView::keyPressEvent(QKeyEvent *e)
{
  if (e->key()==Key_PageDown) {
    QListViewItem *item=firstChild();
    if (item&&!eof()&&item->height()) {
      int num=visibleHeight()/item->height();
      while(num>0) {
	addItem();
	num--;
      }
    }
  }
  QListView::keyPressEvent(e);
}

void toResultView::focusInEvent (QFocusEvent *e)
{
  toMain::editEnable(false,false,true,
		     false,false,
		     false,false,false,true);
  QListView::focusInEvent(e);
}

void toResultView::focusOutEvent (QFocusEvent *e)
{
  toMain::editDisable();
  QListView::focusOutEvent(e);
}

void toResultView::setSQL(toSQL &sql)
{
  SQL=sql(Connection);
  Name=sql.name();
}

void toResultView::query(toSQL &sql)
{
  Name=sql.name();
  query(sql(Connection));
}

#define TORESULT_COPY 1
#define TORESULT_MEMO 2
#define TORESULT_SQL  3

void toResultView::displayMenu(QListViewItem *item,const QPoint &p,int col)
{
  if (item) {
    if (!Menu) {
      Menu=new QPopupMenu(this);
      Menu->insertItem("&Copy",TORESULT_COPY);
      Menu->insertItem("Display in editor",TORESULT_MEMO);
      if (!Name.isEmpty()) {
	Menu->insertSeparator();
	Menu->insertItem("Edit SQL",TORESULT_SQL);
      }
      connect(Menu,SIGNAL(activated(int)),this,SLOT(menuCallback(int)));
    }
    MenuItem=item;
    MenuColumn=col;
    Menu->popup(p);
  }
}

void toResultView::displayMemo(void)
{
  QString str=menuText();
  if (!str.isEmpty())
    new toMemoEditor(this,str);
}

void toResultView::menuCallback(int cmd)
{
  switch(cmd) {
  case TORESULT_COPY:
    {
      QClipboard *clip=qApp->clipboard();
      clip->setText(menuText());
    }
    break;
  case TORESULT_MEMO:
    displayMemo();
    break;
  case TORESULT_SQL:
    toMainWidget()->editSQL(Name);
    break;
  }
}

QString toResultView::menuText(void)
{
  toResultViewItem *resItem=dynamic_cast<toResultViewItem *>(MenuItem);
  toResultViewCheck *chkItem=dynamic_cast<toResultViewCheck *>(MenuItem);
  QString str;
  if (resItem)
    str=resItem->allText(MenuColumn);
  else if (chkItem)
    str=chkItem->allText(MenuColumn);
  else if (MenuItem)
    str=MenuItem->text(MenuColumn);
  return str;
}
