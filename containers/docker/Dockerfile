FROM  eicweb.phy.anl.gov:4567/whit/image_recipes/root_base:latest
LABEL maintainer "Whitney Armstrong <warmstrong@anl.gov>"
#

RUN source /usr/local/bin/thisroot.sh \
  && export PYTHONPATH=/usr/local/lib:$PYTHONPATH  \
  && git clone https://gitlab+deploy-token-1:FaNA-Yg4s7hpjvWPZnq8@eicweb.phy.anl.gov/upsilon/Pcsim.git \
  && mkdir Pcsim/build && cd Pcsim/build  \
  && cmake ../.  \
  && make -j20 \
  && make install


#-DCMAKE_CXX_FLAGS=" -march=haswell -O3 -mfma -malign-data=cacheline -finline-functions "
#&& wget -O- https://root.cern.ch/download/root_v6.14.06.source.tar.gz | tar -zxvf - \
#&& mv root-6.14.06 root_master \
#RUN which c++ && ls -lrth /usr/bin/c++ && cd /tmp/builds/root_build && make -j38 VERBOSE=1 && make install \
#      && cd /tmp && rm -rf /tmp/root_master && rm -rf /tmp/builds/root_build 

#RUN useradd -ms /bin/bash -d /opt/user user
#USER user
#WORKDIR /opt/bubble_user

##CMD ["-c" ]
#ENTRYPOINT ["/bin/bash"]

