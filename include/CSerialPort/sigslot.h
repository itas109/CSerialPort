// sigslot.h: Signal/Slot classes
// Written by Sarah Thompson (sarah@telergy.com) 2002
// more info : http://sigslot.sourceforge.net/
// ** only reserve no paramete function for CSerialPort 4.0 2019-07-21 itas109 **

#ifndef SIGSLOT_H__
#define SIGSLOT_H__

#include <set>
#include <list>

#if defined(SIGSLOT_PURE_ISO) || (!defined(_WIN32) && !defined(__GNUG__) && !defined(SIGSLOT_USE_POSIX_THREADS))
#	define _SIGSLOT_SINGLE_THREADED
#elif defined(_WIN32)
#	define _SIGSLOT_HAS_WIN32_THREADS
#	include <windows.h>
#elif defined(__GNUG__) || defined(SIGSLOT_USE_POSIX_THREADS)
#	define _SIGSLOT_HAS_POSIX_THREADS
#	include <pthread.h>
#else
#	define _SIGSLOT_SINGLE_THREADED
#endif

#ifndef SIGSLOT_DEFAULT_MT_POLICY
#	ifdef _SIGSLOT_SINGLE_THREADED
#		define SIGSLOT_DEFAULT_MT_POLICY single_threaded
#	else
#		define SIGSLOT_DEFAULT_MT_POLICY multi_threaded_local
#	endif
#endif


namespace sigslot {

	class single_threaded
	{
	public:
		single_threaded()
		{
			;
		}

		virtual ~single_threaded()
		{
			;
		}

		virtual void lock()
		{
			;
		}

		virtual void unlock()
		{
			;
		}
	};

#ifdef _SIGSLOT_HAS_WIN32_THREADS
	// The multi threading policies only get compiled in if they are enabled.
	class multi_threaded_global
	{
	public:
		multi_threaded_global()
		{
			static bool isinitialised = false;

			if(!isinitialised)
			{
				InitializeCriticalSection(get_critsec());
				isinitialised = true;
			}
		}

		multi_threaded_global(const multi_threaded_global&)
		{
			;
		}

		virtual ~multi_threaded_global()
		{
			;
		}

		virtual void lock()
		{
			EnterCriticalSection(get_critsec());
		}

		virtual void unlock()
		{
			LeaveCriticalSection(get_critsec());
		}

	private:
		CRITICAL_SECTION* get_critsec()
		{
			static CRITICAL_SECTION g_critsec;
			return &g_critsec;
		}
	};

	class multi_threaded_local
	{
	public:
		multi_threaded_local()
		{
			InitializeCriticalSection(&m_critsec);
		}

		multi_threaded_local(const multi_threaded_local&)
		{
			InitializeCriticalSection(&m_critsec);
		}

		virtual ~multi_threaded_local()
		{
			DeleteCriticalSection(&m_critsec);
		}

		virtual void lock()
		{
			EnterCriticalSection(&m_critsec);
		}

		virtual void unlock()
		{
			LeaveCriticalSection(&m_critsec);
		}

	private:
		CRITICAL_SECTION m_critsec;
	};
#endif // _SIGSLOT_HAS_WIN32_THREADS

#ifdef _SIGSLOT_HAS_POSIX_THREADS
	// The multi threading policies only get compiled in if they are enabled.
	class multi_threaded_global
	{
	public:
		multi_threaded_global()
		{
			pthread_mutex_init(get_mutex(), NULL);
		}

		multi_threaded_global(const multi_threaded_global&)
		{
			;
		}

		virtual ~multi_threaded_global()
		{
			;
		}

		virtual void lock()
		{
			pthread_mutex_lock(get_mutex());
		}

		virtual void unlock()
		{
			pthread_mutex_unlock(get_mutex());
		}

	private:
		pthread_mutex_t* get_mutex()
		{
			static pthread_mutex_t g_mutex;
			return &g_mutex;
		}
	};

	class multi_threaded_local
	{
	public:
		multi_threaded_local()
		{
			pthread_mutex_init(&m_mutex, NULL);
		}

		multi_threaded_local(const multi_threaded_local&)
		{
			pthread_mutex_init(&m_mutex, NULL);
		}

		virtual ~multi_threaded_local()
		{
			pthread_mutex_destroy(&m_mutex);
		}

		virtual void lock()
		{
			pthread_mutex_lock(&m_mutex);
		}

		virtual void unlock()
		{
			pthread_mutex_unlock(&m_mutex);
		}

	private:
		pthread_mutex_t m_mutex;
	};
#endif // _SIGSLOT_HAS_POSIX_THREADS

	template<class mt_policy>
	class lock_block
	{
	public:
		mt_policy *m_mutex;

		lock_block(mt_policy *mtx)
			: m_mutex(mtx)
		{
			m_mutex->lock();
		}

		~lock_block()
		{
			m_mutex->unlock();
		}
	};

	template<class mt_policy>
	class has_slots;

	template<class mt_policy>
	class _connection_base0
	{
	public:
		virtual has_slots<mt_policy>* getdest() const = 0;
        virtual void _emit() = 0;
		virtual _connection_base0* clone() = 0;
		virtual _connection_base0* duplicate(has_slots<mt_policy>* pnewdest) = 0;
	};
	
	template<class mt_policy>
	class _signal_base : public mt_policy
	{
	public:
		virtual void slot_disconnect(has_slots<mt_policy>* pslot) = 0;
		virtual void slot_duplicate(const has_slots<mt_policy>* poldslot, has_slots<mt_policy>* pnewslot) = 0;
	};

	template<class  mt_policy = SIGSLOT_DEFAULT_MT_POLICY>
	class has_slots : public mt_policy 
	{
	private:
		typedef typename std::set<_signal_base<mt_policy> *> sender_set;
		typedef typename sender_set::const_iterator const_iterator;

	public:
		has_slots()
		{
			;
		}

		has_slots(const has_slots& hs)
			: mt_policy(hs)
		{
			lock_block<mt_policy> lock(this);
			const_iterator it = hs.m_senders.begin();
			const_iterator itEnd = hs.m_senders.end();

			while(it != itEnd)
			{
				(*it)->slot_duplicate(&hs, this);
				m_senders.insert(*it);
				++it;
			}
		} 

		void signal_connect(_signal_base<mt_policy>* sender)
		{
			lock_block<mt_policy> lock(this);
			m_senders.insert(sender);
		}

		void signal_disconnect(_signal_base<mt_policy>* sender)
		{
			lock_block<mt_policy> lock(this);
			m_senders.erase(sender);
		}

		virtual ~has_slots()
		{
			disconnect_all();
		}

		void disconnect_all()
		{
			lock_block<mt_policy> lock(this);
			const_iterator it = m_senders.begin();
			const_iterator itEnd = m_senders.end();

			while(it != itEnd)
			{
				(*it)->slot_disconnect(this);
				++it;
			}

			m_senders.erase(m_senders.begin(), m_senders.end());
		}
    
	private:
		sender_set m_senders;
	};

	template<class mt_policy>
	class _signal_base0 : public _signal_base<mt_policy>
	{
	public:
		typedef typename std::list<_connection_base0<mt_policy> *>  connections_list;
		typedef typename connections_list::const_iterator const_iterator;
		typedef typename connections_list::iterator iterator;

		_signal_base0()
		{
			;
		}

		_signal_base0(const _signal_base0& s)
			: _signal_base<mt_policy>(s)
		{
			lock_block<mt_policy> lock(this);
			const_iterator  it = s.m_connected_slots.begin();
			const_iterator itEnd = s.m_connected_slots.end();

			while(it != itEnd)
			{
				(*it)->getdest()->signal_connect(this);
				m_connected_slots.push_back((*it)->clone());

				++it;
			}
		}

		~_signal_base0()
		{
			disconnect_all();
		}

		void disconnect_all()
		{
			lock_block<mt_policy> lock(this);
			const_iterator it  = m_connected_slots.begin();
			const_iterator itEnd = m_connected_slots.end();

			while(it != itEnd)
			{
				(*it)->getdest()->signal_disconnect(this);
				delete *it;

				++it;
			}

			m_connected_slots.erase(m_connected_slots.begin(), m_connected_slots.end());
		}

		void disconnect(has_slots<mt_policy>* pclass)
		{
			lock_block<mt_policy> lock(this);
			iterator it = m_connected_slots.begin();
			iterator itEnd = m_connected_slots.end();

			while(it != itEnd)
			{
				if((*it)->getdest() == pclass)
				{
					delete *it;
					m_connected_slots.erase(it);
					pclass->signal_disconnect(this);
					return;
				}

				++it;
			}
		}

		void slot_disconnect(has_slots<mt_policy>* pslot)
		{
			lock_block<mt_policy> lock(this);
			iterator it = m_connected_slots.begin();
			iterator itEnd = m_connected_slots.end();

			while(it != itEnd)
			{
				iterator itNext = it;
				++itNext;

				if((*it)->getdest() == pslot)
				{
					m_connected_slots.erase(it);
					//			delete *it;
				}

				it = itNext;
			}
		}

		void slot_duplicate(const has_slots<mt_policy>* oldtarget, has_slots<mt_policy>* newtarget)
		{
			lock_block<mt_policy> lock(this);
			iterator it = m_connected_slots.begin();
			iterator itEnd = m_connected_slots.end();

			while(it != itEnd)
			{
				if((*it)->getdest() == oldtarget)
				{
					m_connected_slots.push_back((*it)->duplicate(newtarget));
				}

				++it;
			}
		}

	protected:
		connections_list m_connected_slots;   
	};

	template<class dest_type, class mt_policy>
	class _connection0 : public _connection_base0<mt_policy>
	{
	public:
		_connection0()
		{
			this->pobject = NULL;
			this->pmemfun = NULL;
		}

		_connection0(dest_type* pobject, void (dest_type::*pmemfun)())
		{
			m_pobject = pobject;
			m_pmemfun = pmemfun;
		}

		virtual _connection_base0<mt_policy>* clone()
		{
			return new _connection0<dest_type, mt_policy>(*this);
		}

		virtual _connection_base0<mt_policy>* duplicate(has_slots<mt_policy>* pnewdest)
		{
			return new _connection0<dest_type, mt_policy>((dest_type *)pnewdest, m_pmemfun);
		}

        virtual void _emit()
		{
			(m_pobject->*m_pmemfun)();
		}

		virtual has_slots<mt_policy>* getdest() const
		{
			return m_pobject;
		}

	private:
		dest_type* m_pobject;
		void (dest_type::* m_pmemfun)();
	};
	
	template<class mt_policy = SIGSLOT_DEFAULT_MT_POLICY>
	class signal0 : public _signal_base0<mt_policy>
	{
	public:
    	typedef typename _signal_base0<mt_policy>::connections_list::const_iterator const_iterator;
		signal0()
		{
			;
		}

		signal0(const signal0<mt_policy>& s)
			: _signal_base0<mt_policy>(s)
		{
			;
		}

		template<class desttype>
			void connect(desttype* pclass, void (desttype::*pmemfun)())
		{
			lock_block<mt_policy> lock(this);
			_connection0<desttype, mt_policy>* conn = 
				new _connection0<desttype, mt_policy>(pclass, pmemfun);
			this->m_connected_slots.push_back(conn);
			pclass->signal_connect(this);
		}

        void _emit()
		{
			lock_block<mt_policy> lock(this);
			const_iterator itNext, it = this->m_connected_slots.begin();
			const_iterator itEnd = this->m_connected_slots.end();

			while(it != itEnd)
			{
				itNext = it;
				++itNext;

                (*it)->_emit();

				it = itNext;
			}
		}

		void operator()()
		{
			lock_block<mt_policy> lock(this);
			const_iterator itNext, it = this->m_connected_slots.begin();
			const_iterator itEnd = this->m_connected_slots.end();

			while(it != itEnd)
			{
				itNext = it;
				++itNext;

                (*it)->_emit();

				it = itNext;
			}
		}
	};

}; // namespace sigslot

#endif // SIGSLOT_H__