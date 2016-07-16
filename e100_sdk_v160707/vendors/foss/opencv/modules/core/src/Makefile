obj-y += libopencv_core.a

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
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc/opencv
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc/opencv2
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
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc/opencv
ccflags-y += -I$(srctree)/vendors/foss/opencv/inc
ccflags-y += -DOPENCV_NOSTL
#pass build check
ccflags-y += -D__OPENCV_BUILD
ccflags-y += -include opencv2/core.hpp


libopencv_core-objs += algorithm.o            convert.o        datastructs.o       kmeans.o     matop.o     pca.o          tables.o\
alloc.o                copy.o           directx.o           lapack.o     matrix.o    persistence.o  types.o\
arithm.o               cuda_gpu_mat.o   downhill_simplex.o  lda.o        ocl.o       rand.o         umatrix.o\
array.o                cuda_host_mem.o  dxt.o               lpsolver.o   opengl.o    stat.o\
cuda_info.o      gl_core_3_1.o       mathfuncs.o  out.o       stl.o\
conjugate_gradient.o   cuda_stream.o    glob.o              matmul.o     parallel.o  system.o

#command_line_parser.o

install-files := libopencv_core.a
install-path :=$(srctree)/vendors/ambarella/lib