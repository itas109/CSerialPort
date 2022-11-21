/**
 * @file CommWXWidgetsMain.h
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief Defines Application Frame (CodeBlocks 20.03 wxWidgets 3.1.7)
 */

#ifndef COMMWXWIDGETSMAIN_H
#define COMMWXWIDGETSMAIN_H

//(*Headers(CommWXWidgetsDialog)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

//About CSerialPort start
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;
//About CSerialPort end

class CommWXWidgetsDialog: public wxDialog, public CSerialPortListener // About CSerialPort
{
    public:

        CommWXWidgetsDialog(wxWindow* parent,wxWindowID id = -1);
        virtual ~CommWXWidgetsDialog();

        void onReadEvent(const char *portName, unsigned int readBufferLen);//About CSerialPort

private:
	CSerialPort m_SerialPort; //About CSerialPort
	unsigned int rx, tx; //About CSerialPort

    private:
        //(*Handlers(CommWXWidgetsDialog)
        void OnButtonOpenCloseClick(wxCommandEvent& event);
        void OnButtonSendClick(wxCommandEvent& event);
        void OnButtonClearCountClick(wxCommandEvent& event);
        //*)


        //(*Identifiers(CommWXWidgetsDialog)
        static const long ID_STATICTEXT_PortName;
        static const long ID_CHOICE_PortName;
        static const long ID_STATICTEXT_Baudrate;
        static const long ID_CHOICE_Baudrate;
        static const long ID_STATICTEXT_Parity;
        static const long ID_CHOICE_Parity;
        static const long ID_STATICTEXT_DataBit;
        static const long ID_CHOICE_DataBit;
        static const long ID_STATICTEXT_StopBit;
        static const long ID_CHOICE_StopBit;
        static const long ID_BUTTON_OpenClose;
        static const long ID_STATICTEXT_Info;
        static const long ID_BUTTON_Send;
        static const long ID_RICHTEXTCTRL_Receive;
        static const long ID_RICHTEXTCTRL_Send;
        static const long ID_STATICTEXT_RX;
        static const long ID_STATICTEXT_RXValue;
        static const long ID_STATICTEXT_TX;
        static const long ID_STATICTEXT_TXValue;
        static const long ID_BUTTON_ClearCount;
        //*)

        //(*Declarations(CommWXWidgetsDialog)
        wxBoxSizer* BoxSizer1;
        wxButton* ButtonClearCount;
        wxButton* ButtonOpenClose;
        wxButton* ButtonSend;
        wxChoice* ChoiceBaudrate;
        wxChoice* ChoiceDataBit;
        wxChoice* ChoiceParity;
        wxChoice* ChoicePortName;
        wxChoice* ChoiceStopBit;
        wxRichTextCtrl* RichTextCtrlReceive;
        wxRichTextCtrl* RichTextCtrlSend;
        wxStaticText* StaticTextBaudrate;
        wxStaticText* StaticTextDataBit;
        wxStaticText* StaticTextInfo;
        wxStaticText* StaticTextParity;
        wxStaticText* StaticTextPortName;
        wxStaticText* StaticTextRX;
        wxStaticText* StaticTextRXValue;
        wxStaticText* StaticTextStopBit;
        wxStaticText* StaticTextTX;
        wxStaticText* StaticTextTXValue;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // COMMWXWIDGETSMAIN_H
