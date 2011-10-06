#pragma once

enum kParameters
{
	kCutoff = 0, 
	kReso,
	kFGain,
	kFVel,
	kFEnv,
	kF12dB,
	kF24dB,
	kFLP,
	kFBP,
	kFHP,
	kFBR,
	kFExp,

	kSemi1, kFine1, kMorph1, kOKTrk1, kOSync1, kOAM1, kONormal1,
	kOAdd1, kOMul1, kOSuper1, kOOff1, kOSaw1, kOPulse1, kOTri1,
	kONoise1, kOSpread1,

	kSemi2, kFine2, kMorph2, kOKTrk2, kOSync2, kOAM2, kONormal2,
	kOAdd2, kOMul2, kOSuper2, kOOff2, kOSaw2, kOPulse2, kOTri2,
	kONoise2, kOSpread2,

	kSemi3, kFine3, kMorph3, kOKTrk3, kOSync3, kOAM3, kONormal3,
	kOAdd3, kOMul3, kOSuper3, kOOff3, kOSaw3, kOPulse3, kOTri3,
	kONoise3, kOSpread3,

	kMixVol,
	kMixVel,
	kMixPan,
	kMixVol1,
	kMixVol2,
	kMixVol3,

	kVoices,
	kGlide,

	kAttack1,
	kHold1,
	kDecay1,
	kSustain1,
	kRelease1,

	kAttack2,
	kHold2,
	kDecay2,
	kSustain2,
	kRelease2,

	kAttack3,
	kHold3,
	kDecay3,
	kSustain3,
	kRelease3,

	kL1Speed,
	kL1Delay,
	kL1PW,
	kL1KTrack,
	kL1Off,
	kL1Saw,
	kL1Pulse,
	kL1Tri,
	kL1Noise,
	kL1Sync,
	kL1SNH,
	kL1Retrig,

	kL2Speed,
	kL2Delay,
	kL2PW,
	kL2KTrack,
	kL2Off,
	kL2Saw,
	kL2Pulse,
	kL2Tri,
	kL2Noise,
	kL2Sync,
	kL2SNH,
	kL2Retrig,

	kM1Source,
	kM1Amount,
	kM1Dest,

	kM2Source,
	kM2Amount,
	kM2Dest,

	kM3Source,
	kM3Amount,
	kM3Dest,

	kM4Source,
	kM4Amount,
	kM4Dest,

	kM5Source,
	kM5Amount,
	kM5Dest,

	kM6Source,
	kM6Amount,
	kM6Dest,

	kArpSpeed,
	kArpLength,
	kArpNote1,
	kArpNote2,
	kArpNote3,
	kArpNote4,
	kArpNote5,
	kArpNote6,
	kArpNote7,
	kArpNote8,
	kArpSync,
	kArpLoop,

	kPanic,
	kPatchStore,
	kPatchRecall,
	kPatchChaos,

	kAudioVol,
	kAudioPan,

	kNumParameters
};

enum pParameters
{
	pOSemi1,
	pOFine1,
	pOMorph1,
	pOWave1,
	pOMode1,
	pOFlags1,
	pOSpread1,

	pOSemi2,
	pOFine2,
	pOMorph2,
	pOWave2,
	pOMode2,
	pOFlags2,
	pOSpread2,

	pOSemi3,
	pOFine3,
	pOMorph3,
	pOWave3,
	pOMode3,
	pOFlags3,
	pOSpread3,

	pAttack1,
	pHold1,
	pDecay1,
	pSustain1,
	pRelease1,

	pAttack2,
	pHold2,
	pDecay2,
	pSustain2,
	pRelease2,

	pAttack3,
	pHold3,
	pDecay3,
	pSustain3,
	pRelease3,

	pMixVol,
	pMixPan,
	pMixVelsens,
	pMixVol1,
	pMixVol2,
	pMixVol3,

	pFCutoff,
	pFQ,
	pFVelsens,
	pFGain,
	pFEnv2,
	pFMode,
	pFType,
	pFExp,

	pVoices,
	pGlide,

	pArpSpeed,
	pArpLength,
	pArpNote1,
	pArpNote2,
	pArpNote3,
	pArpNote4,
	pArpNote5,
	pArpNote6,
	pArpNote7,
	pArpNote8,
	pArpSync,
	pArpLoop,

	pL1Speed,
	pL1Delay,
	pL1PW,
	pL1Keytrack,
	pL1Wave,
	pL1Sync,
	pL1Retrig,
	pL1SNH,

	pL2Speed,
	pL2Delay,
	pL2PW,
	pL2Keytrack,
	pL2Wave,
	pL2Sync,
	pL2Retrig,
	pL2SNH,

	pM1Source,
	pM1Amount,
	pM1Dest,

	pM2Source,
	pM2Amount,
	pM2Dest,

	pM3Source,
	pM3Amount,
	pM3Dest,

	pM4Source,
	pM4Amount,
	pM4Dest,

	pM5Source,
	pM5Amount,
	pM5Dest,

	pM6Source,
	pM6Amount,
	pM6Dest,

	pAftertouch,
	pModwheel,

	pAudioVol,
	pAudioPan,

	pNumParameters
};

#define OBJ_COUNT kNumParameters
