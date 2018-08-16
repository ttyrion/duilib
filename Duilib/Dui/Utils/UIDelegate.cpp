#include "StdAfx.h"

namespace DuiLib {

CDelegateBase::CDelegateBase(void* pObject, void* pFn) 
{
    m_pObject = pObject;
    m_pFn = pFn; 
}

CDelegateBase::CDelegateBase(const CDelegateBase& rhs) 
{
    m_pObject = rhs.m_pObject;
    m_pFn = rhs.m_pFn; 
}

CDelegateBase::~CDelegateBase()
{

}

bool CDelegateBase::Equals(const CDelegateBase& rhs) const 
{
    return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool CDelegateBase::operator() (void* param) 
{
    return Invoke(param); 
}

void* CDelegateBase::GetFn() 
{
    return m_pFn; 
}

void* CDelegateBase::GetObject() 
{
    return m_pObject; 
}

CEventSource::~CEventSource()
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject) delete pObject;
    }
}

CEventSource::operator bool()
{
    return m_aDelegates.GetSize() > 0;
}

void CEventSource::operator+= (const CDelegateBase& d)
{ 
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) return;
    }

    m_aDelegates.Add(d.Copy());
}

void CEventSource::operator+= (FnType pFn)
{ 
    (*this) += MakeDelegate(pFn);
}

void CEventSource::operator-= (const CDelegateBase& d) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) {
            delete pObject;
            m_aDelegates.Remove(i);
            return;
        }
    }
}
void CEventSource::operator-= (FnType pFn)
{ 
    (*this) -= MakeDelegate(pFn);
}

bool CEventSource::operator() (void* param) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && !(*pObject)(param) ) return false;
    }
    return true;
}

CEventSets::CEventSets()
{

}

CEventSets::~CEventSets()
{
    EventMap::iterator it = events_.begin();
    for (auto& iter : events_) {
        if (iter.second) {
            delete iter.second;
        }
    }

    events_.clear();
}

void CEventSets::Subscribe(const EventType name, CDelegateBase& subscriber) {
    CEventSource * source = GetEventObject(name, true);
    *source += subscriber;
}

bool CEventSets::UnSubscribe(const EventType name, CDelegateBase& subscriber) {
    CEventSource* obj = GetEventObject(name, false);
    if (obj != NULL) {
        *obj -= subscriber;

        return true;
    }

    return false;
}

CEventSource* CEventSets::GetEventObject(const EventType name, bool gen_if_none) {
    EventMap::iterator pos = events_.find(name);
    if (pos == events_.end()) {
        if (gen_if_none) {
            AddEvent(name);
            return events_.find(name)->second;
        }
        else {
            return NULL;
        }            
    }

    return pos->second;
}

void CEventSets::AddEvent(const EventType name) {
    CEventSource* eventObj = new CEventSource();
    events_[name] = eventObj;
}

bool CEventSets::FireEvent(const EventType name, void* event_param) {
    CEventSource* obj = GetEventObject(name, false);
    if (obj != NULL)
        return (*obj)(event_param);
    return true;
}

} // namespace DuiLib
