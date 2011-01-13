#define HEXES_HEXAPP_CPP_

#include <ncurses.h>

#include "HexApp.h"


namespace hexes {

bool HexApp::NCURSES_LIBINIT = false;

HexApp::HexApp()
    : _echo(false)
{
    this->InitCurses(false, false);
    _hasColors = ::has_colors();
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
    int r = 0;

    if ( _col != COLS || _row != LINES ) {
        _col = COLS;
        _row = LINES;
        this->rescale();
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

        r = p->redraw();

        if ( r == 0 ) {
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
    
    update_panels();
    doupdate();
    refresh();

    return 1;
}

int
HexApp::rescale() 
{
    return 1;
}

void
HexApp::setTopPanel ( HexPanel * panel )
{
    panel->setTopPanel();
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

    return p;
}


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


void
HexApp::InitCurses ( bool termRaw, bool echo )
{
    if ( NCURSES_LIBINIT )
        return;

    initscr();

    if ( termRaw )
        raw();
    else
        cbreak();

    if ( ! echo )
        noecho();

    start_color();
    keypad(stdscr, TRUE);
    HexApp::NCURSES_LIBINIT = true;

    return;
}


}  // namespace

// HEXES_HEXAPP_CPP_

