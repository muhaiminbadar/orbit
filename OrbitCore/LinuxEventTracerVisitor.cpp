#include "OrbitProcess.h"
#include "ContextSwitch.h"
#include "TimerManager.h"
#include "LinuxEventTracerVisitor.h"
#include "LinuxPerfEvent.h"
#include "PrintVar.h"

void LinuxEventTracerVisitor::visit(LinuxPerfLostEvent* a_Event)
{
    PRINT("Lost %u Events\n", a_Event->Lost());
}

void LinuxEventTracerVisitor::visit(LinuxForkEvent* a_Event)
{
    if (m_Process->HasThread(a_Event->ParentTID()))
    {
        m_Process->AddThreadId(a_Event->TID());
    }
}

void LinuxEventTracerVisitor::visit(LinuxSchedSwitchEvent* a_Event)
{
    // the known thread stopped running
    if (m_Process->HasThread(a_Event->PrevPID()))
    {
        ++Capture::GNumContextSwitches;

        ContextSwitch CS ( ContextSwitch::Out );
        CS.m_ThreadId = a_Event->PrevPID();
        CS.m_Time = a_Event->Timestamp();
        CS.m_ProcessorIndex = a_Event->CPU();
        //TODO: Is this correct?
        CS.m_ProcessorNumber = a_Event->CPU();
        GTimerManager->Add( CS );
    }

    // the known thread starts running
    if (m_Process->HasThread(a_Event->NextPID()))
    {
        ++Capture::GNumContextSwitches;

        ContextSwitch CS ( ContextSwitch::In );
        CS.m_ThreadId = a_Event->NextPID();
        CS.m_Time = a_Event->Timestamp();
        CS.m_ProcessorIndex = a_Event->CPU();
        //TODO: Is this correct?
        CS.m_ProcessorNumber = a_Event->CPU();
        GTimerManager->Add( CS );
    }
}