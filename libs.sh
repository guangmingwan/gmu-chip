mipsel-linux-ldd decoders/*.so |grep "not found"|grep "=>"|awk '{print $1}'|sort|uniq
