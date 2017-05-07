/****************************************
 *Author: magang
 *Date: 2014.5.4
 *Function: circular queue template class 
 ***************************************/
#ifndef _CIRCULAR_QUEUE_H
#define _CIRCULAR_QUEUE_H

template<class T>
class CCircularQueue
{
	public:
		CCircularQueue();
		~CCircularQueue();
		bool init(unsigned int uiMaxSize);
		bool empty()const;
		bool full()const;
		unsigned int size()const;
		T& front()const;
		void next();
		void reset();
		T& back()const;
		void pop();
		void push(const T& element);

	private:
		unsigned int m_uiMaxSize;
		unsigned int m_uiSize;
		int m_iFirst;
		int m_iLast;
		int m_iResetPos;
		T* m_pArra;
};

template<class T>
CCircularQueue<T>::CCircularQueue()
{
	m_uiMaxSize = 0;
   	m_uiSize = 0;
   	m_iFirst = 0;
   	m_iLast = 0;
	m_pArra = NULL;
	m_iResetPos = 0;
}

template<class T>
CCircularQueue<T>::~CCircularQueue()
{
	if (m_pArra != NULL) {
		delete []m_pArra;
		m_pArra = NULL;
	}
}

template<class T>
bool CCircularQueue<T>::init(unsigned int uiMaxSize)
{
	m_pArra = new T[uiMaxSize];
	if (m_pArra == NULL) {
		return false;
	}
	m_uiMaxSize = uiMaxSize;

	return true;
}

template<class T>
inline unsigned int CCircularQueue<T>::size()const
{
	return m_uiSize;
}

template<class T>
inline bool CCircularQueue<T>::empty()const
{
	return (m_uiSize == 0);
}

template<class T>
inline bool CCircularQueue<T>::full()const
{
	return (m_uiSize == m_uiMaxSize);
}

template<class T> 
inline T& CCircularQueue<T>::front()const
{
	return m_pArra[m_iFirst];
}

template<class T> 
inline void CCircularQueue<T>::next()
{
	m_iResetPos = m_iFirst++;
	m_iFirst %= m_uiMaxSize;
}

template<class T> 
inline void CCircularQueue<T>::reset()
{
	m_iFirst = m_iResetPos;
}
	
template<class T> 
inline T& CCircularQueue<T>::back()const
{
	return m_pArra[m_iLast - 1];
}

template<class T> 
inline void CCircularQueue<T>::push(const T& element)
{
	if (!full()) {
		m_iLast %= m_uiMaxSize;
		m_pArra[m_iLast++] = element;
		m_uiSize++;
	}
}

template<class T> 
inline void CCircularQueue<T>::pop()
{
	if (!empty()) {
		m_iFirst++;
		m_iFirst %= m_uiMaxSize;
		m_uiSize--;
	}
}

#endif

