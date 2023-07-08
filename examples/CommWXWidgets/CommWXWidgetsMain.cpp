/**
 * @file CommWXWidgetsMain.cpp
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief Code for Application Frame (CodeBlocks 20.03 wxWidgets 3.1.7)
 */

#include "CommWXWidgetsMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(CommWXWidgetsDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(CommWXWidgetsDialog)
const long CommWXWidgetsDialog::ID_STATICTEXT_PortName = wxNewId();
const long CommWXWidgetsDialog::ID_CHOICE_PortName = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_Baudrate = wxNewId();
const long CommWXWidgetsDialog::ID_CHOICE_Baudrate = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_Parity = wxNewId();
const long CommWXWidgetsDialog::ID_CHOICE_Parity = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_DataBit = wxNewId();
const long CommWXWidgetsDialog::ID_CHOICE_DataBit = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_StopBit = wxNewId();
const long CommWXWidgetsDialog::ID_CHOICE_StopBit = wxNewId();
const long CommWXWidgetsDialog::ID_BUTTON_OpenClose = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_Info = wxNewId();
const long CommWXWidgetsDialog::ID_BUTTON_Send = wxNewId();
const long CommWXWidgetsDialog::ID_RICHTEXTCTRL_Receive = wxNewId();
const long CommWXWidgetsDialog::ID_RICHTEXTCTRL_Send = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_RX = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_RXValue = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_TX = wxNewId();
const long CommWXWidgetsDialog::ID_STATICTEXT_TXValue = wxNewId();
const long CommWXWidgetsDialog::ID_BUTTON_ClearCount = wxNewId();
//*)

BEGIN_EVENT_TABLE(CommWXWidgetsDialog,wxDialog)
    //(*EventTable(CommWXWidgetsDialog)
    //*)
END_EVENT_TABLE()

CommWXWidgetsDialog::CommWXWidgetsDialog(wxWindow* parent,wxWindowID id)
: rx(0)
, tx(0)
{
    //(*Initialize(CommWXWidgetsDialog)
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer3;
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer8;
    wxBoxSizer* BoxSizer9;
    wxBoxSizer* BoxSizerPortName;
    wxStaticBoxSizer* StaticBoxSizerReceive;
    wxStaticBoxSizer* StaticBoxSizerSend;
    wxStaticBoxSizer* StaticBoxSizerSetting;

    Create(parent, wxID_ANY, _("CommWXWidgets"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizerSetting = new wxStaticBoxSizer(wxVERTICAL, this, _("SerialPort Setting"));
    BoxSizerPortName = new wxBoxSizer(wxHORIZONTAL);
    StaticTextPortName = new wxStaticText(this, ID_STATICTEXT_PortName, _("PortName"), wxDefaultPosition, wxSize(94,21), 0, _T("ID_STATICTEXT_PortName"));
    BoxSizerPortName->Add(StaticTextPortName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 10);
    ChoicePortName = new wxChoice(this, ID_CHOICE_PortName, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_PortName"));
    BoxSizerPortName->Add(ChoicePortName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerSetting->Add(BoxSizerPortName, 1, wxALL|wxEXPAND, 5);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticTextBaudrate = new wxStaticText(this, ID_STATICTEXT_Baudrate, _("Baudrate"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_Baudrate"));
    BoxSizer5->Add(StaticTextBaudrate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ChoiceBaudrate = new wxChoice(this, ID_CHOICE_Baudrate, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_Baudrate"));
    ChoiceBaudrate->Append(_("300"));
    ChoiceBaudrate->Append(_("600"));
    ChoiceBaudrate->Append(_("1200"));
    ChoiceBaudrate->Append(_("2400"));
    ChoiceBaudrate->Append(_("4800"));
    ChoiceBaudrate->SetSelection( ChoiceBaudrate->Append(_("9600")) );
    ChoiceBaudrate->Append(_("14400"));
    ChoiceBaudrate->Append(_("19200"));
    ChoiceBaudrate->Append(_("38400"));
    ChoiceBaudrate->Append(_("56000"));
    ChoiceBaudrate->Append(_("57600"));
    ChoiceBaudrate->Append(_("115200"));
    BoxSizer5->Add(ChoiceBaudrate, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerSetting->Add(BoxSizer5, 1, wxALL|wxEXPAND, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    StaticTextParity = new wxStaticText(this, ID_STATICTEXT_Parity, _("Parity"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_Parity"));
    BoxSizer6->Add(StaticTextParity, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ChoiceParity = new wxChoice(this, ID_CHOICE_Parity, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_Parity"));
    ChoiceParity->SetSelection( ChoiceParity->Append(_("None")) );
    ChoiceParity->Append(_("Odd"));
    ChoiceParity->Append(_("Even"));
    ChoiceParity->Append(_("Mark"));
    ChoiceParity->Append(_("Space"));
    BoxSizer6->Add(ChoiceParity, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerSetting->Add(BoxSizer6, 1, wxALL|wxEXPAND, 5);
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    StaticTextDataBit = new wxStaticText(this, ID_STATICTEXT_DataBit, _("DataBit"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_DataBit"));
    BoxSizer7->Add(StaticTextDataBit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ChoiceDataBit = new wxChoice(this, ID_CHOICE_DataBit, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_DataBit"));
    ChoiceDataBit->Append(_("5"));
    ChoiceDataBit->Append(_("6"));
    ChoiceDataBit->Append(_("7"));
    ChoiceDataBit->SetSelection( ChoiceDataBit->Append(_("8")) );
    BoxSizer7->Add(ChoiceDataBit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerSetting->Add(BoxSizer7, 1, wxALL|wxEXPAND, 5);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    StaticTextStopBit = new wxStaticText(this, ID_STATICTEXT_StopBit, _("StopBit"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_StopBit"));
    BoxSizer8->Add(StaticTextStopBit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ChoiceStopBit = new wxChoice(this, ID_CHOICE_StopBit, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_StopBit"));
    ChoiceStopBit->SetSelection( ChoiceStopBit->Append(_("1")) );
    ChoiceStopBit->Append(_("1.5"));
    ChoiceStopBit->Append(_("2"));
    BoxSizer8->Add(ChoiceStopBit, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerSetting->Add(BoxSizer8, 1, wxALL|wxEXPAND, 5);
    ButtonOpenClose = new wxButton(this, ID_BUTTON_OpenClose, _("open"), wxDefaultPosition, wxSize(284,41), 0, wxDefaultValidator, _T("ID_BUTTON_OpenClose"));
    StaticBoxSizerSetting->Add(ButtonOpenClose, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizerSetting, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    StaticTextInfo = new wxStaticText(this, ID_STATICTEXT_Info, _("Author: itas109\nhttps://github.com/itas109/CSerialPort\nhttps://blog.csdn.net/itas109"), wxDefaultPosition, wxSize(299,66), 0, _T("ID_STATICTEXT_Info"));
    BoxSizer4->Add(StaticTextInfo, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonSend = new wxButton(this, ID_BUTTON_Send, _("Send"), wxDefaultPosition, wxSize(293,80), 0, wxDefaultValidator, _T("ID_BUTTON_Send"));
    BoxSizer4->Add(ButtonSend, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizerReceive = new wxStaticBoxSizer(wxVERTICAL, this, _("Receive"));
    RichTextCtrlReceive = new wxRichTextCtrl(this, ID_RICHTEXTCTRL_Receive, wxEmptyString, wxDefaultPosition, wxSize(348,271), wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL_Receive"));
    wxRichTextAttr rchtxtAttr_1;
    rchtxtAttr_1.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
    StaticBoxSizerReceive->Add(RichTextCtrlReceive, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizerReceive, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizerSend = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Send"));
    RichTextCtrlSend = new wxRichTextCtrl(this, ID_RICHTEXTCTRL_Send, _("https://blog.csdn.net/itas109"), wxDefaultPosition, wxSize(337,120), wxRE_MULTILINE, wxDefaultValidator, _T("ID_RICHTEXTCTRL_Send"));
    wxRichTextAttr rchtxtAttr_2;
    rchtxtAttr_2.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
    StaticBoxSizerSend->Add(RichTextCtrlSend, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizerSend, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    StaticTextRX = new wxStaticText(this, ID_STATICTEXT_RX, _("RX:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_RX"));
    BoxSizer9->Add(StaticTextRX, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticTextRXValue = new wxStaticText(this, ID_STATICTEXT_RXValue, _("0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_RXValue"));
    BoxSizer9->Add(StaticTextRXValue, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticTextTX = new wxStaticText(this, ID_STATICTEXT_TX, _("TX: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_TX"));
    BoxSizer9->Add(StaticTextTX, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticTextTXValue = new wxStaticText(this, ID_STATICTEXT_TXValue, _("0"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT_TXValue"));
    BoxSizer9->Add(StaticTextTXValue, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ButtonClearCount = new wxButton(this, ID_BUTTON_ClearCount, _("CL"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_ClearCount"));
    ButtonClearCount->SetMinSize(wxSize(50,-1));
    ButtonClearCount->SetMaxSize(wxSize(50,-1));
    BoxSizer9->Add(ButtonClearCount, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer9, 0, wxALL|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_BUTTON_OpenClose,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommWXWidgetsDialog::OnButtonOpenCloseClick);
    Connect(ID_BUTTON_Send,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommWXWidgetsDialog::OnButtonSendClick);
    Connect(ID_BUTTON_ClearCount,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommWXWidgetsDialog::OnButtonClearCountClick);
    //*)

    // set title
    wxString title = wxString::Format("%s %s", GetLabel(), m_SerialPort.getVersion());
    SetLabel(title);

    // get avaiable ports
    std::vector<SerialPortInfo> m_portsList = CSerialPortInfo::availablePortInfos();
	for (size_t i = 0; i < m_portsList.size(); i++)
	{
	    if(0 == i)
        {
            ChoicePortName->SetSelection(ChoicePortName->Append(m_portsList[i].portName));
        }
        else
        {
            ChoicePortName->Append(m_portsList[i].portName);
        }
	}

    // bind receive function
    m_SerialPort.connectReadEvent(this);
}

CommWXWidgetsDialog::~CommWXWidgetsDialog()
{
    //(*Destroy(CommWXWidgetsDialog)
    //*)
}

void CommWXWidgetsDialog::onReadEvent(const char *portName, unsigned int readBufferLen)
{
    if (readBufferLen > 0)
    {
        char *data = new char[readBufferLen + 1]; // '\0'

        if (data)
        {
            // read
            int recLen = m_SerialPort.readData(data, readBufferLen);

            if (recLen > 0)
            {
                data[recLen] = '\0';

                RichTextCtrlReceive->AppendText(data);

                rx += recLen;
                StaticTextRXValue->SetLabel(wxString::Format(wxT("%i"), rx));
            }

            delete[] data;
            data = NULL;
        }
    }
}

void CommWXWidgetsDialog::OnButtonOpenCloseClick(wxCommandEvent& event)
{
    if(!m_SerialPort.isOpen())
    {
        // wx3.0 not support wxString::ToInt
        long baudrate,DataBits = 0;
        ChoiceBaudrate->GetString(ChoiceBaudrate->GetCurrentSelection()).ToLong(&baudrate);
        ChoiceDataBit->GetString(ChoiceDataBit->GetCurrentSelection()).ToLong(&DataBits);
        m_SerialPort.init(ChoicePortName->GetString(ChoicePortName->GetCurrentSelection()).ToStdString().c_str(),
                          baudrate,
                          itas109::Parity(ChoiceParity->GetCurrentSelection()),
                          itas109::DataBits(DataBits),
                          itas109::StopBits(ChoiceStopBit->GetCurrentSelection())
                          );
        m_SerialPort.open();

        if(m_SerialPort.isOpen())
        {
            ButtonOpenClose->SetLabel("close");
        }
        else
        {
            wxMessageBox(wxString::Format("open error\ncode: %i, message: %s\nPortName: %s\nBaudrate: %i\nParity: %i\nDataBits: %i\nStopBits: %i",
                                          m_SerialPort.getLastError(),
                                          m_SerialPort.getLastErrorMsg(),
                                          m_SerialPort.getPortName(),
                                          m_SerialPort.getBaudRate(),
                                          m_SerialPort.getParity(),
                                          m_SerialPort.getDataBits(),
                                          m_SerialPort.getStopBits()
                                          ));
        }
    }
    else
    {
        m_SerialPort.close();

        ButtonOpenClose->SetLabel("open");
    }
}

void CommWXWidgetsDialog::OnButtonSendClick(wxCommandEvent& event)
{
    if(!m_SerialPort.isOpen())
    {
        wxMessageBox("Please open serial port first");
        return;
    }

    std::string data = RichTextCtrlSend->GetValue().ToStdString();
    if(data.size() > 0)
    {
        int ret = m_SerialPort.writeData(data.c_str(),data.size());

        tx += ret;
        StaticTextTXValue->SetLabel(wxString::Format(wxT("%i"),tx));
    }

}

void CommWXWidgetsDialog::OnButtonClearCountClick(wxCommandEvent& event)
{
    rx = 0;
    tx = 0;

    StaticTextRXValue->SetLabel("0");
    StaticTextTXValue->SetLabel("0");
}
