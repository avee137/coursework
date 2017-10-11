#!/usr/bin/env python
import threading
import time
import logging

logging.basicConfig(level=logging.DEBUG, 
					format='[%(levelname)s], %(threadName)-10s, %(message)s',)

def worker_thread(n):
	#print("this is a worker thread %s" %(threading.currentThread.getName()))
    logging.debug('starting')
    time.sleep(2)
    logging.debug('Exiting')
	#print threading.currentThread().getName()

threads = []
def spawn_threads(n):
	for i in range (n):
		t = threading.Thread(name='t'+str(i),target=worker_thread, args=(i,))
		t.start()
		threads.append(t)

if __name__ == "__main__":
	#print("hello")
	spawn_threads(5)
	for i in range(5):
		threads[i].join() #wait until the thread does its work and joins the flow here i.e returns
	print("goodbye")	
	

