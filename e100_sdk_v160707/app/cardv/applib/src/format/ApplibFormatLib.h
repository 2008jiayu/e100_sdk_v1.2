#ifndef __FORMAT_LIB_H__

#include <format/Format.h>




extern int ApplibFormatLib_ResetDmxMediaInfo(AMP_MEDIA_INFO_s *media);




void ApplibFormatLib_InitPTS(AMP_MEDIA_TRACK_INFO_s *track);

UINT64 ApplibFormatLib_ConvertPTS(AMP_MEDIA_TRACK_INFO_s *track, AMP_BITS_DESC_s *frame);

BOOL ApplibFormatLib_IsVideoComplete(AMP_MOVIE_INFO_s *movie);

BOOL ApplibFormatLib_CheckEnd(AMP_MEDIA_INFO_s *media);

void ApplibFormatLib_UpdateMuxTrack(AMP_MEDIA_TRACK_INFO_s *track, UINT32 frameCount);

#ifdef CONFIG_APP_ARD
AMP_MEDIA_TRACK_INFO_s *ApplibFormatLib_GetDefaultTrack(AMP_MEDIA_INFO_s *media, UINT8 trackType);

AMP_MEDIA_TRACK_INFO_s *ApplibFormatLib_GetShortestTrack(AMP_MEDIA_INFO_s *media);

int ApplibFormatLib_RestoreDTS(AMP_MEDIA_INFO_s *media);

#ifdef CONFIG_APP_EVENT_OVERLAP
int AppLibFormatMuxMp4_EventRecord_event(void);
int AppLibFormatMuxMp4_GetEventStatus(void);
void AppLibFormatMuxMp4_EventStop(void);
void AppLibFormatMuxMp4_EventParkingMode_Status(int status);
#endif
#endif

#define __FORMAT_LIB_H__
#endif
