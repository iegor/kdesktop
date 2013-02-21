// (c) 2000 Peter Putzer
#ifndef KSCROLLER_H
#define KSCROLLER_H

#include <qframe.h>

class QScrollBar;

/**
 * A window that automatically enables scrollbars
 * if it's smaller than the content.
 *
 * @author Peter Putzer <putzer@kde.org>
 * @version $Id: kscroller.h 142720 2002-03-13 07:06:50Z waba $
 */
class KScroller : public QFrame
{
  Q_OBJECT

public:
  /**
   * Constructor.
   *
   * @param parent the parent of this widget (passed on as ususal).
   * @param name the name of this widget (as above).
   */
  KScroller (QWidget* parent = 0L, const char* name = 0L);

  /** 
   * Destructor.
   */
  virtual ~KScroller ();

  /**
   * @return the vertical scrollbar.
   */
  inline QScrollBar* verticalScrollBar () { return mVertical; }

  /**
   * @return the horizontal scrollbar.
   */
  inline QScrollBar* horizontalScrollBar () { return mHorizontal; }

  /**
   * @return the current content, or 0L if none set.
   */
  inline QWidget* content () { return mContent; }

  /**
   * @return the current corner widget, or 0L if none set.
   */
  QWidget* cornerWidget ();

public slots:
  /**
   * Sets the content. Ownership is transfered to the scroller, any
   * previously set content will be deleted!
   * 
   * @param content has to be a child of the KScroller.
   */
  void setContent (QWidget* content);

  /**
   * Sets the corner widget (the small widget that's located where the vertical and horizontal scrollbars
   * allmost meet).
   *
   * @param corner has to be a child of the KScroller.
   */
  void setCornerWidget (QWidget* corner);

  /**
   * Update the scrollbars. Call whenever you change the contents minimumSize.
   */
  void updateScrollBars ();

protected:
  /**
   * Reimplemented for internal reasons, the API is not affected.
   */
  virtual void resizeEvent (QResizeEvent*);

  /**
   * Reimplemented for internal reasons, the API is not affected.
   */
  virtual QSize sizeHint() const;

  /**
   * Reimplemented for internal reasons, the API is not affected.
   */
  virtual QSize minimumSizeHint() const;


private slots:
  /**
   * Scroll vertically.
   *
   * @param value is the new slider value.
   */
  void scrollVertical (int value);

  /**
   * Scroll horizontally.
   *
   * @param value is the new slider value.
   */
  void scrollHorizontal (int value);

private:
  /**
   * Set up the horizontal scrollbar.
   *
   * @param cw is the width of the content.
   * @param ch is the height of the content.
   * @param w is the width of the scroller.
   * @param h is the height of the scroller.
   */
  void setupHorizontal (int cw, int ch, int w, int h);

  /**
   * Set up the vertical scrollbar
   *
   * @param cw is the width of the content.
   * @param ch is the height of the content.
   * @param w is the width of the scroller.
   * @param h is the height of the scroller.
   */
  void setupVertical (int cw, int ch, int w, int h);

  /**
   * Set up the corner widget.
   *
   * @param w is the width of the scroller.
   * @param h is the height of the scroller.
   */
  void setupCornerWidget (int w, int h);

  class KScrollerPrivate;
  KScrollerPrivate* d;

  QScrollBar* mVertical;
  QScrollBar* mHorizontal;
  QWidget* mCornerWidget;

  QWidget* mContent;
  
  int mVerticalOld;
  int mHorizontalOld;
};

#endif // KSCROLLER_H
