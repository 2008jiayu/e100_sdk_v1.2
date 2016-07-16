obj-y += libopencv_objdetect.a

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/fs
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/prfile
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaTest
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw/net/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/ituner
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/imgschdlr
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/imgproc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/imgcalib
ccflags-y += -I$(srctree)/vendors/ambarella/inc/va
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/peripheral/sensor/inc
ccflags-y += -I$(srctree)/peripheral/lcd/inc
ccflags-y += -I$(srctree)/peripheral/audio/inc
ccflags-y += -I$(srctree)/peripheral/inc/misc
ccflags-y += -I$(src)/extSrc/tune
ccflags-y += -I$(src)/extSrc/sample_calib
ccflags-y += -I$(src)/extSrc/sample_iqparam_handler
ccflags-y += -std=gnu99
ccflags-y += -fshort-wchar
ccflags-y += -frtti
ccflags-y += -fexceptions

ccflags-y += -I$(src)/../include
ccflags-y += -I$(src)/..
#cvconfig.h
ccflags-y += -I$(src)/../../..
#opencv2/core/opencl/ocl_defs
ccflags-y += -I$(src)/../../core/include
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc/opencv
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc
ccflags-y += -DOPENCV_NOSTL
#pass build check
ccflags-y += -D__OPENCV_BUILD
ccflags-y += -include opencv2/objdetect.hpp


libopencv_objdetect-objs += cascadedetect.o
libopencv_objdetect-objs += cascadedetect_convert.o
libopencv_objdetect-objs += detection_based_tracker.o
libopencv_objdetect-objs += haar.o
libopencv_objdetect-objs += hog.o

install-files := libopencv_objdetect.a
install-path :=$(srctree)/vendors/ambarella/lib