// ============================================================================
// $Id$

//
// = LIBRARY
//    tests
// 
// = FILENAME
//    Task_Test.cpp
//
// = DESCRIPTION
//      This test program illustrates how the ACE barrier
//      synchronization mechanisms work in conjunction with the
//      ACE_Task and the ACE_Thread_Manager.
//
// = AUTHOR
//    Prashant Jain and Doug C. Schmidt
// 
// ============================================================================

#include "ace/Log_Msg.h"

#if defined (ACE_HAS_THREADS)

#include "ace/Service_Config.h"
#include "ace/Task.h"
#include "test_config.h"

class Barrier_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
  Barrier_Task (ACE_Thread_Manager *thr_mgr,
		int n_threads, 
		int n_iterations);
  
  virtual int svc (void);
  // Iterate <n_iterations> time printing off a message and "waiting"
  // for all other threads to complete this iteration.

private:
  ACE_Barrier barrier_;
  // Reference to the tester barrier.  This controls each
  // iteration of the tester function running in every thread.

  int n_iterations_;
  // Number of iterations to run.

  // = Not needed for this test.
  virtual int open (void *) { return 0; }
  virtual int close (u_long) { return 0; }
  virtual int put (ACE_Message_Block *, ACE_Time_Value *) { return 0; }
};

Barrier_Task::Barrier_Task (ACE_Thread_Manager *thr_mgr, 
			    int n_threads, 
			    int n_iterations)
  : ACE_Task<ACE_MT_SYNCH> (thr_mgr), 
    barrier_ (n_threads), 
    n_iterations_ (n_iterations) 
{
  // Create worker threads.
  if (this->activate (THR_NEW_LWP, n_threads) == -1)
    ACE_ERROR ((LM_ERROR, "%p\n", "activate failed"));
}
  
// Iterate <n_iterations> time printing off a message and "waiting"
// for all other threads to complete this iteration.

int 
Barrier_Task::svc (void) 
{  
  // Note that the ACE_Task::svc_run() method automatically adds us to
  // the Thread_Manager when the thread begins.
  ACE_NEW_THREAD;

  for (int iterations = 1; 
       iterations <= this->n_iterations_;
       iterations++)
    {
      ACE_DEBUG ((LM_DEBUG, "(%t) in iteration %d\n", iterations));

      // Block until all other threads have waited, then continue.
      this->barrier_.wait ();
    }

  // Note that the ACE_Task::svc_run() method automatically removes us
  // from the Thread_Manager when the thread exits.

  return 0;
}

int 
main (int argc, char *argv[])
{
  ACE_START_TEST ("Task_Test.cpp");

  int n_threads = ACE_MAX_THREADS;
  int n_iterations = ACE_MAX_ITERATIONS;

  Barrier_Task barrier_task (ACE_Service_Config::thr_mgr (), 
			     n_threads, 
			     n_iterations);

  // Wait for all the threads to reach their exit point.
  ACE_Service_Config::thr_mgr ()->wait ();

  ACE_END_TEST;
  return 0;
}
#else
int 
main (int, char *[])
{
  ACE_ERROR ((LM_ERROR, "threads not supported on this platform\n"));
  return 0;
}
#endif /* ACE_HAS_THREADS */
