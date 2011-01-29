#define HEXES_HEXAPP_CPP_

#include <ncurses.h>

#include "HexApp.h"


namespace hexes {

bool HexApp::_CURSESINIT = false;

HexApp::HexApp()
    : _curPanel(NULL),
      _echo(false)
{
    this->InitCurses(false, _echo);
    _hasColor = ::has_colors();
    _col      = LINES;
    _row      = COLS;
}


HexApp::~HexApp()
{
    this->destroyPanels();
    nocbreak();
    endwin();
}


int
HexApp::draw()
{
    int  result   = 0;
    bool rescaled = false;

    if ( _col != COLS || _row != LINES ) {
        _col     = COLS;
        _row     = LINES;
        rescaled = true;
    }

    StringList  removes;

    PanelMap::iterator  pIter;
    for ( pIter = _panels.begin(); pIter != _panels.end(); ++pIter )
    {
        HexPanel * p = pIter->second;

        if ( p == NULL ) {
            removes.push_back(pIter->first);
            continue;
        }

        if ( rescaled )
            p->resize(_row, _col);

        result = p->redraw();

        if ( result == 0 ) {
            p->erase();
            removes.push_back(pIter->first);
        }
    }

    StringList::iterator sIter;

    for ( sIter = removes.begin(); sIter != removes.end(); ++sIter ) {
        pIter = _panels.find(*sIter);
        if ( pIter == _panels.end() )
            continue;
        if ( pIter->second )
            delete pIter->second;
        _panels.erase(pIter);
    }
    removes.clear();
    
    //set current panel
    //reset cursor pos.
    
    ::update_panels();
    ::doupdate();
    ::refresh();

    return 1;
}


int
HexApp::poll()
{
    int r = ERR;
    
    if ( _curPanel != NULL )
        r = _curPanel->poll();

    if ( r != ERR )
        return r;

    PanelMap::iterator pIter;
    for ( pIter = _panels.begin(); pIter != _panels.end(); ++pIter )
    {
        r = pIter->second->poll();
        if ( r != ERR )
            break;
    }
    
    return r;
}



void
HexApp::setTopPanel ( HexPanel * panel )
{
    panel->setTopPanel();
    _curPanel = panel;
}


HexPanel*
HexApp::createPanel ( const std::string & title, 
                      int height, int width, 
                      int starty, int startx )
{
    HexPanel * p = NULL;
    PanelMap::iterator pIter;

    pIter = _panels.find(title);
    if ( pIter != _panels.end() )
        return p;

    p = new HexPanel(title, height, width, starty, startx);

    _panels[title] = p;
    if ( _curPanel == NULL )
        this->setTopPanel(p);

    return p;
}

/**  Adds the HexPanel to this HexApp instance. By doing so, 
  *  the HexApp instance will 'own' the panel, calling the 
  *  proper I/O events on redraw and poll, and freeing the 
  *  object on destruction.
 **/
bool
HexApp::addPanel ( HexPanel * panel )
{
    if ( panel == NULL )
        return false;

    PanelMap::iterator pIter;

    pIter = _panels.find(panel->getTitle());

    if ( pIter != _panels.end() )
        return false;
    
    _panels[panel->getTitle()] = panel;

    return true;
}

/** Removes the panel identified by @param title and returns 
  * the pointer to the corresponding HexPanel. HexApp will no 
  * longer track the panel object and it will not be free'd by
  * the HexApp instance. Returns NULL if no such panel exists.
 **/
HexPanel*
HexApp::removePanel ( const std::string & title )
{
    HexPanel * panel = NULL;
    PanelMap::iterator pIter;

    pIter = _panels.find(title);
    if ( pIter == _panels.end() )
        return panel;

    panel = pIter->second;
    _panels.erase(pIter);

    return panel;
}

/**  Removes and destroys(free) the HexPanel instance identified 
  *  by @param title. Return false if no such panel exists.
 **/
bool
HexApp::destroyPanel ( const std::string & title )
{
    PanelMap::iterator pIter;

    pIter = _panels.find(title);
    if ( pIter == _panels.end() )
        return false;

    delete pIter->second;
    _panels.erase(pIter);

    return true;
}

/** Destroys all panels tracked by this instance of HexApp */
void
HexApp::destroyPanels()
{
    PanelMap::iterator pIter;

    for ( pIter = _panels.begin(); pIter != _panels.end(); ++pIter )
    {
        if ( pIter->second != NULL )
            delete pIter->second;
    }
    _panels.clear();

    return;
}


int
HexApp::getMaxWidth()
{
    return this->width();
}

int
HexApp::width()
{
    return _col;
}

int
HexApp::height()
{
    return _row;
}

int
HexApp::getMaxHeight()
{
    return this->height();
}


bool
HexApp::hasColor() const
{
    return _hasColor;
}

int
HexApp::print ( int y, int x, const std::string & str )
{
    return(mvwaddstr(stdscr, y, x, str.c_str()));
}

void
HexApp::InitCurses ( bool termRaw, bool echo )
{
    if ( HexApp::_CURSESINIT )
        return;

    initscr();

    if ( termRaw )
        raw();
    else
        cbreak();

    if ( ! echo )
        noecho();

    nonl();
    start_color();
    keypad(stdscr, TRUE);

    HexApp::_CURSESINIT = true;

    return;
}

std::string
HexApp::Version()
{
    std::string ver = LIBHEXES_VERSION;
    return ver;
}


}  // namespace

// HEXES_HEXAPP_CPP_

