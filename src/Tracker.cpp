#include "Tracker.h"
#include <iostream>


NvMOTStatus NvMOT_Query(uint16_t customConfigFilePathSize,
                         char* pCustomConfigFilePath,
                         NvMOTQuery *pQuery)
{
    /**
     * Users can parse the low-level config file in pCustomConfigFilePath to check
     * the low-level tracker's requirements
     */

    /**  Required configs for all custom trackers. */
    pQuery->computeConfig = NVMOTCOMP_GPU;       // among {NVMOTCOMP_GPU, NVMOTCOMP_CPU}
    pQuery->numTransforms = 0;                   // 0 for IOU and NvSORT tracker, 1 for NvDCF or NvDeepSORT tracker as they require the video frames
    pQuery->colorFormats[0] = NVBUF_COLOR_FORMAT_NV12; // among {NVBUF_COLOR_FORMAT_NV12, NVBUF_COLOR_FORMAT_RGBA}

    // among {NVBUF_MEM_DEFAULT, NVBUF_MEM_CUDA_DEVICE, NVBUF_MEM_CUDA_UNIFIED, NVBUF_MEM_CUDA_PINNED, ... }
    #ifdef __aarch64__
        pQuery->memType = NVBUF_MEM_DEFAULT;
    #else
        pQuery->memType = NVBUF_MEM_CUDA_DEVICE;
    #endif

    pQuery->batchMode = NvMOTBatchMode_Batch;    // batchMode must be set as NvMOTBatchMode_Batch
    pQuery->maxTargetsPerStream = MAX_TARGETS_PER_STREAM; // Max number of targets stored for each stream

    /** Optional configs to set for additional features. */
    pQuery->supportPastFrame = false;             // Set true only if the low-level tracker supports the past-frame data
    pQuery->maxShadowTrackingAge = 30;           // Maximum length of shadow tracking, required if supportPastFrame is true
    pQuery->outputReidTensor = false;             // Set true only if the low-level tracker supports outputting reid feature
    pQuery->reidFeatureSize = 256;               // Size of Re-ID feature, required if outputReidTensor is true

    std::cout << "[BYTETrack Initialized]" << std::endl;
    return NvMOTStatus_OK;
}

NvMOTStatus NvMOT_Init(NvMOTConfig *pConfigIn,
                         NvMOTContextHandle *pContextHandle,
                         NvMOTConfigResponse *pConfigResponse)
{
    if(pContextHandle != nullptr)
    {
        NvMOT_DeInit(*pContextHandle);
    }

    /// User-defined class for the context
    NvMOTContext *pContext = nullptr;

    /// Instantiate the user-defined context
    pContext = new NvMOTContext(*pConfigIn, *pConfigResponse);

    /// Pass the pointer as the context handle
    *pContextHandle = pContext;

    return NvMOTStatus_OK;
}


void NvMOT_DeInit(NvMOTContextHandle contextHandle)
{
    /// Destroy the context handle
    delete contextHandle;
}


NvMOTStatus NvMOT_Process(NvMOTContextHandle contextHandle,
                          NvMOTProcessParams *pParams,
                          NvMOTTrackedObjBatch *pTrackedObjectsBatch)
{
    /// Process the given video frame using the user-defined method in the context, and generate outputs
    return contextHandle->processFrame(pParams, pTrackedObjectsBatch);
}


NvMOTStatus NvMOT_RetrieveMiscData(NvMOTContextHandle contextHandle,
                              NvMOTProcessParams *pParams,
                              NvMOTTrackerMiscData *pTrackerMiscData)
{
    /// Retrieve the past-frame data if there are
    return contextHandle->retrieveMiscData(pParams, pTrackerMiscData);
}


NvMOTStatus NvMOT_RemoveStreams(NvMOTContextHandle contextHandle,
                                   NvMOTStreamId streamIdMask)
{
    /// Remove the specified video stream from the low-level tracker context
    return contextHandle->removeStream(streamIdMask);
}
