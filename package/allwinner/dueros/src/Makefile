all:
	sh build.sh build/build.conf
	
install:
	#-mkdir -p   $(INSTALL_PREFIX)/sbin
	#-cp sbin/*  $(INSTALL_PREFIX)/sbin/
	mkdir -p   $(INSTALL_PREFIX)/home
	cp OMtech_buildout/*  $(INSTALL_PREFIX)/home/

.PHONY:all clean $(TARGET) install
