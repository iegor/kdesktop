#include "mwidget.h"
#include "autorouter.h"

#include <arts/debug.h>

//#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qtimer.h>
#include <qpainter.h>
#include <qevent.h>

#include "createtool.h"

Structure *ModuleWidget::theStructure()
{
	return structure;
}

void ModuleWidget::addInterface ( const Arts::ModuleInfo& minfo )
{
	delete activeTool;
	activeTool = new CreateInterfaceTool(this, minfo);
}

void ModuleWidget::addModule ( const Arts::ModuleInfo& minfo )
{
	delete activeTool;
	activeTool = new CreateModuleTool(this, minfo);
}

void ModuleWidget::addPort ( const Arts::PortType& type )
{
	delete activeTool;
	activeTool = new CreatePortTool(this, type);
}

StructurePort *ModuleWidget::insertPort( const Arts::PortType& type, int x, int y )
{
	StructurePort *port = structure->createStructurePort(type);
	unselectAll();
	port->move(x, y);
	port->setSelected(true);
	port->show();

	return port;
}

void ModuleWidget::leaveTool(MWidgetTool *tool, bool wasModified)
{
	assert(tool == activeTool);
	delete tool;
	activeTool = 0;
	if(wasModified)
		emit modified(wasModified);
}

QPoint ModuleWidget::componentPos(const StructureComponent *component) const
{
	int cellx = 0, celly = 0;
	colXPos(component->x(), &cellx);
	rowYPos(component->y(), &celly);

	return QPoint(cellx, celly);
}

QPoint ModuleWidget::portPos(const ModulePort *port) const
{
	int cellx = 0, celly = 0;
	colXPos(port->owner->x() + port->drawsegment, &cellx);
	rowYPos(port->owner->y(), &celly);

	return QPoint(cellx, celly);
}

bool ModuleWidget::insertModule( Module *newModule )
{
	if(hasSpace(newModule, newModule->x(), newModule->y(), true))
	{
		newModule->show();
		reRoute();
		return true;
	}
	return false;
}

void ModuleWidget::findAt(int windowX, int windowY,
						  StructureComponent *&component, ModulePort *&port)
{
	int x = findCol(windowX);
	int y = findRow(windowY);

	component = structure->componentAt(x, y, false);

	if(component)
	{
		int cellx = 0, celly = 0;
		colXPos(x, &cellx);
		rowYPos(y, &celly);

		port = component->portAt(x - (component->x()),
								windowX - cellx, windowY - celly);
	}
	else
		port = 0L;
}

void ModuleWidget::selectComponent( StructureComponent *component, bool onlyThis )
{
	beginUpdate();
	if(onlyThis)
		unselectAll();

	if(!(component->selected()))
	{
		component->setSelected(true);
		emit componentSelected(component);
	} else
		if(!onlyThis)
		{
			component->setSelected(false);
			emit componentSelected(0L);
		}
	endUpdate();
}

void ModuleWidget::mousePressEvent( QMouseEvent *e )
{
	if(activeTool)
	{
		activeTool->mousePressEvent(e);
		return;
	}

	if( e->button() == LeftButton )
	{
		StructureComponent *component;
		ModulePort *port;
		findAt(e->x(), e->y(), component, port);

		if(component)
		{
			if(port)
			{
				// user clicked in port
				selectPort(port);

				delete activeTool;
				activeTool = new ConnectPortsTool(this, port);
			}
			else
			{
				// user clicked in component
				activeTool = new MoveComponentsTool(this, e);

				// maintain selected group when pressing the shift or control button
				selectComponent(component, !((e->state() & ControlButton)
											 || (e->state() & ShiftButton)));
			}
		}
		else
		{
			// unselect all if user clicks on background (without shift)
			if(!(e->state() & ShiftButton))
			{
				beginUpdate();
				unselectAll();
				endUpdate();
			}
		}
	}
}

void ModuleWidget::mouseMoveEvent( QMouseEvent *e )
{
	if(activeTool)
	{
		activeTool->mouseMoveEvent(e);
		return;
	}
}

void ModuleWidget::mouseReleaseEvent( QMouseEvent *e )
{
	if(activeTool)
	{
		activeTool->mouseReleaseEvent(e);
		return;
	}
}

// may be called with port == 0
void ModuleWidget::selectPort( ModulePort *port, bool newMode )
{
	beginUpdate();

	if(selectedPort && (selectedPort!= port))
	{
		// unselect previous
		selectedPort->selected = false;
		selectedPort->owner->redraw();
		if(selectedPort->owner->selected())
			emit componentSelected(selectedPort->owner);
		else
			emit portSelected(0L);
		selectedPort = 0L;
	}

	if(port)
	{
		port->selected = newMode;
		selectedPort = port;
		selectComponent(selectedPort->owner);
		selectedPort->owner->redraw();
	}
	emit portSelected(port); // FIXME: should be "portSelectionChanged"

	endUpdate();
}

void ModuleWidget::startConnection( ModulePort *port )
{
	delete activeTool;
	activeTool = new ConnectPortsTool(this, port);
}

void ModuleWidget::portPropertiesChanged( ModulePort *port )
{
	reRoute();
}

bool ModuleWidget::hasSpace(StructureComponent *c, int destx, int desty,
							bool ignore_selected)
{
	if((destx < 0) || (desty < 0))
		return false;
	if((destx + c->width() > numCols()) || (desty + c->height() > numRows()))
		return false;

	for(int ddx = 0; ddx < c->width(); ddx++)
	{
		for(int ddy = 0; ddy < c->height(); ddy++)
		{
			if(structure->componentAt(destx + ddx, desty + ddy, ignore_selected))
				return false;
		}
	}

	return true;
}

void ModuleWidget::paintCellBackground(QPainter *p, int y, int x)
{
	QColor bgcolor;

	if((y & 1) == 1)
		bgcolor = QColor(168, 168, 168);
	else
		bgcolor = QColor(146, 168, 146);

	p->fillRect(0, 0, cellsize, cellsize, QBrush(bgcolor));

	p->setPen(bgcolor.dark(115));
	p->drawLine(0, 0, 0, cellsize - 1);
	p->drawLine(0, 0, cellsize - 1, 0);

	if(x == (numCols() - 1))
		p->drawLine(cellsize - 1, 0, cellsize - 1, cellsize - 1);
	if(y == (numRows() - 1))
		p->drawLine(0, cellsize - 1, cellsize - 1, cellsize - 1);
}

void ModuleWidget::unselectAll()
{
	setSelectAll(false);
}

void ModuleWidget::setSelectAll(bool newstate)
{
	std::list<StructureComponent *>::iterator module;

	for(module = structure->getComponentList()->begin();
        module != structure->getComponentList()->end(); module++)
	{
		(*module)->setSelected(newstate);
	}
}

void ModuleWidget::beginUpdate()
{
	updateDepth++;
}

void ModuleWidget::endUpdate()
{
	if(!--updateDepth)
	{
		std::list<QRect>::iterator i;

		for(i = UpdateList.begin(); i != UpdateList.end(); i++)
		{
			redrawCells(*i);
		}

		UpdateList.clear();
	}
}

void ModuleWidget::redrawRect(int x, int y, int width, int height)
{
	QRect r = QRect(x, y, width, height);

	if(!updateDepth)
	{
		redrawCells(r);
	}
	else
	{
		UpdateList.push_back(r);
	}
}

void ModuleWidget::redrawCells(QRect &r)
{
	int x, y;

	for(x = r.left(); x <= r.right(); x++)
	{
		for(y = r.top(); y <= r.bottom(); y++)
		{
			updateCell(y, x, false);
		}
	}
}


void ModuleWidget::reRoute()
{
// clear everything
	autorouter->clear();

// add structure components (external ports/modules) to the router, so that
// cables won't be drawn over them

	std::list<StructureComponent *>::iterator c;
	std::list<ModulePort *> portlist;

	for(c = structure->getComponentList()->begin();
		c != structure->getComponentList()->end(); c++)
	{
		StructureComponent *sc = *c;
		autorouter->set(sc->x()*2, sc->y()*2,
						(sc->x() + sc->width())*2 - 1,
						(sc->y() + sc->height())*2 - 1, AutoRouter::solid);

		sc->dumpPorts(portlist);
	}

	std::list<ModulePort *>::iterator pi;

// build a map with all input ports to find corresponding ports of connections

	std::map<long, ModulePort *> portmap;

	for(pi = portlist.begin(); pi != portlist.end(); ++pi)
	{
		ModulePort *port = *pi;

		if(port->direction == ModulePort::in) portmap[port->pdID] = port;
	}

// add connections to the router

	/*
	 * assign colors after the following algorithm:
	 *
	 * - initialize assuming that each port has its own color
	 * - if two ports are connected, they must have the same color, that
	 *   is, all references to the two colors must be made the same
	 *
	 * these colors are not used for drawing, but for making lines of
	 * different groups of ports not collide in the autorouter (ownership)
	 */
	std::map<ModulePort *, long> color;
	vector<long> owner(portlist.size());

	long maxcolor = 0;
	for(pi = portlist.begin(); pi != portlist.end(); ++pi)
		color[*pi] = maxcolor++;

	for(pi = portlist.begin(); pi != portlist.end(); ++pi)
	{
		ModulePort *src = *pi;
		unsigned long c;
		if(src->direction == ModulePort::out && src->PortDesc.isConnected())
		{
			vector<Arts::PortDesc> *conn = src->PortDesc.connections();

			for(c = 0; c < conn->size(); c++)
			{
				ModulePort *dest = portmap[(*conn)[c].ID()];
				long color_src = color[src];
				long color_dest = color[dest];

				if(color_src != color_dest)
				{
					std::list<ModulePort *>::iterator pi2;

					for(pi2 = portlist.begin(); pi2 != portlist.end(); pi2++)
					{
						ModulePort *p = *pi2;

						if(color[p] == color_dest) color[p] = color_src;
					}
				}
			}
		}
	}

	for(pi = portlist.begin(); pi != portlist.end(); ++pi)
	{
		ModulePort *p = *pi;

		if(p->direction == ModulePort::out && p->PortDesc.isConnected())
		{
			ModulePort *src = p, *dest;
			long& route_owner = owner[color[src]];
			unsigned long c;

			vector<Arts::PortDesc> *conn = p->PortDesc.connections();

			for(c = 0; c < conn->size(); c++)
			{
				dest = portmap[(*conn)[c].ID()];
				if(dest) // otherwise something bad has happend?
				{
/*
						arts_debug("autoroute add connection port %s.%s to %s.%s",
							src->owner->type.local8Bit().data(),src->description.local8Bit().data(),
							dest->owner->type.local8Bit().data(),dest->description.local8Bit().data());
*/

					int x1 = (src->owner->x() + src->drawsegment)*2;
					int y1 = src->owner->y()*2 + 1;

					int x2 = (dest->owner->x() + dest->drawsegment)*2;
					int y2 = dest->owner->y()*2;

					route_owner = autorouter->connect(x1, y1, x2, y2, route_owner);
				}
			}

			delete conn;
		}
	}

	autorouter->sync();
}

void ModuleWidget::redrawAll()
{
// redraw everything
	QRect updaterect(0, 0, cols, rows);
	redrawCells(updaterect);
}

void ModuleWidget::paintConnection(QPainter *p, int x, int y, int arx, int ary)
{
	long linetype = autorouter->get(arx, ary);
	long ud_owner = -1, lr_owner = -1, lr_break = 0, ud_break = 0;

	autorouter->getowners(arx, ary, ud_owner, lr_owner);

	p->setPen(QColor(255, 255, 255));

/*
	if(linetype == AutoRouter::none)
	{
		p->drawPoint(x + cellsize/4, y + cellsize/4);
	}
	if(linetype & AutoRouter::solid)
	{
		QBrush whitefill(QColor(255, 255, 255));

		p->fillRect(x + cellsize/6, y + cellsize/6, cellsize/6, cellsize/6, whitefill);
	}
*/
	x += cellsize/4;
	y += cellsize/4;

	// both used?
	if(ud_owner != -1 && lr_owner != -1)
	{
		// and not of the same owner?
		if(ud_owner != lr_owner)
		{
			// then we'll have to paint one of them broken
			if(ud_owner > lr_owner)
				lr_break = cellsize/8;
			else
				ud_break = cellsize/8;
		}
	}

	if(linetype & AutoRouter::left)
		p->drawLine(x - cellsize/4, y, x - lr_break, y);
	if(linetype & AutoRouter::right)
		p->drawLine(x + cellsize/4, y, x + lr_break, y);
	if(linetype & AutoRouter::up)
		p->drawLine(x, y - cellsize/4, x, y - ud_break);
	if(linetype & AutoRouter::down)
		p->drawLine(x, y + cellsize/4, x, y + ud_break);
}

void ModuleWidget::paintConnections(QPainter *p, int y, int x)
{
	// paints connections in the given 2x2-autorouter-block being a 1x1 block to the user
	for(int dx = 0; dx < 2; dx++)
		for(int dy = 0; dy < 2; dy++)
			paintConnection(p, (cellsize*dx)/2, (cellsize*dy)/2, x*2 + dx, y*2 + dy);
}

void ModuleWidget::paintCell(QPainter *p, int y, int x)
{
#if 0 /* PORT */
	if(theArtsBuilderApp->eventStackDepth() > 1)
	{
		// FIXME: set some redraw flag or something like that
		return;
	}
#endif

	std::list<StructureComponent *>::iterator c;
	for(c = structure->getComponentList()->begin();
		c != structure->getComponentList()->end(); c++)
	{
		StructureComponent *mwc = *c;
		if(y == mwc->y() && mwc->visible())
		{
			int xoffset = x - mwc->x();

			if(xoffset >= 0 && xoffset < mwc->width())
			{
				if(mwc->drawNeedsBackground(xoffset))
					 paintCellBackground(p, y, x);

				mwc->drawSegment(p, cellsize, xoffset);
				paintConnections(p, y, x);
				return;
			}
		}
	}
	paintCellBackground(p, y, x);
	paintConnections(p, y, x);
}

// ---------------------------------------------------------------------------
// public part of modulewidget
// ---------------------------------------------------------------------------

void ModuleWidget::setZoom(int zoom)
{
	cellsize = (int)(50.0 * (float)zoom/100);

	setCellHeight(cellsize);
	setCellWidth(cellsize);
	updateTableSize();
	resize(cellsize*cols, cellsize*rows);
}

void ModuleWidget::selectAll()
{
	setSelectAll(true);
}

void ModuleWidget::reInit()
{
	emit componentSelected(0);
	selectedPort = 0L;

	delete activeTool;
	activeTool = 0L;

	reRoute();
}

void ModuleWidget::delModule()
{
	int numSelected = structure->countSelected();

	if(!numSelected) return;

	if(KMessageBox::warningContinueCancel(0,
		i18n("Delete %n selected module, port or connection? (No undo possible.)",
		     "Delete %n selected modules, ports and connections? (No undo possible.)",
		     numSelected), QString::null, i18n("&Delete")) == KMessageBox::Continue)
	{
		selectPort(0L);
		emit componentSelected(0);
		structure->deleteSelected();
		reRoute();
	}
}

void ModuleWidget::autoRedrawRouter()
{
	if(autorouter->needRedraw()) redrawAll();
}

ModuleWidget::ModuleWidget(Structure *structure, QWidget *parent, const char *name, WFlags f)
	: QtTableView( parent, name, f),
	  updateDepth( 0 ),
	  activeTool( 0L ),
	  structure( structure ),
	  selectedPort( 0L )
{
	arts_debug("PORT: mw; getmodulelist");
	this->ModuleList = structure->getModuleList();
	arts_debug("PORT: mw; cols&rows");

	cols = 24;
	rows = 32;

	setNumCols(cols);
	setNumRows(rows);
	setTableFlags(Tbl_autoScrollBars);
	setZoom(100);

	setFocusPolicy( NoFocus );

	arts_debug("PORT: mw; bgmode");
	setBackgroundMode(NoBackground);

	arts_debug("PORT: mw; new ar %d,%d", cols, rows);
	autorouter = new AutoRouter(cols*2, rows*2);

	arts_debug("PORT: mw; new ar ok - qtimer");
    QTimer *timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()),
		this, SLOT(autoRedrawRouter()) );

	arts_debug("PORT: mw; tstart");
	timer->start( 100, FALSE );                 // 100 ms reoccurring check
}

ModuleWidget::~ModuleWidget()
{
	delete autorouter;
}

#include "mwidget.moc"
