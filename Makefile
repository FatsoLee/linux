obj-m = m_pid.o
default:
	$(MAKE) -Wall -C /usr/src/kernels/2.6.18-238.el5-i686/ M=/usr/src/linux/ modules
clean:
	$(MAKE) -Wall -C /usr/src/kernels/2.6.18-238.el5-i686/ M=/usr/src/linux/ clean

