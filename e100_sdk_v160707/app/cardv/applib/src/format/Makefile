
libapplib-objs +=src/format/ApplibFormat.o
libapplib-objs +=src/format/ApplibFormat_DemuxExif.o
libapplib-objs +=src/format/ApplibFormat_DemuxMp4.o
libapplib-objs +=src/format/ApplibFormat_MuxerManager.o
libapplib-objs +=src/format/ApplibFormat_MuxExif.o
ifdef CONFIG_APP_EVENT_OVERLAP
libapplib-objs +=src/format/ApplibFormat_MuxMp4_EVENT_O.o
else
libapplib-objs +=src/format/ApplibFormat_MuxMp4.o
endif
libapplib-objs +=src/format/ApplibFormatLib.o
libapplib-objs +=src/format/ApplibFormat_MuxExifPIV.o
ifdef CONFIG_APP_EVENT_OVERLAP
libapplib-objs +=src/format/ApplibPrecMux.o
else
libapplib-objs +=src/format/ApplibFormat_MuxEvent.o
endif