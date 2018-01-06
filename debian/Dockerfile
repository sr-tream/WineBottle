FROM debian

RUN apt update && \
	apt upgrade -y && \
	apt install -y qt5-default git hashdeep wget make g++-multilib

RUN mkdir /root/WineBottle

RUN echo "#\!/bin/bash\n\n" >> /bin/build.sh && \
	echo "cd /root/WineBottle\n" >> /bin/build.sh && \
	echo "git pull\n" >> /bin/build.sh && \
	echo "mkdir build\n" >> /bin/build.sh && \
	echo "cd build\n" >> /bin/build.sh && \
	echo "qmake ..\n" >> /bin/build.sh && \
	echo "make -j$(nproc)\n" >> /bin/build.sh && \
	echo "cp /root/WineBottle/debian/makedeb.sh ./makedeb.sh\n" >> /bin/build.sh && \
	echo "chmod 755 makedeb.sh\n" >> /bin/build.sh && \
	echo "./makedeb.sh" >> /bin/build.sh && \
	echo "cp WineBottle /root/WineBottle/debian/build/bin/" >> /bin/build.sh && \
	echo "cp winebottle_* /root/WineBottle/debian/build/" >> /bin/build.sh && \
	echo "rm -rf /root/WineBottle/build" >> /bin/build.sh

RUN chmod 755 /bin/build.sh

CMD ["/bin/bash", "/bin/build.sh"]
