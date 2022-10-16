/**
 * @file CommWXWidgetsApp.cpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief Code for Application Class (CodeBlocks 20.03 wxWidgets 3.1.7)
 */

#include "CommWXWidgetsApp.h"

//(*AppHeaders
#include "CommWXWidgetsMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(CommWXWidgetsApp);

bool CommWXWidgetsApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	CommWXWidgetsDialog Dlg(0);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }
    //*)
    return wxsOK;

}
