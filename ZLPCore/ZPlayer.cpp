#include "ZPlayer.h"

#include <d3d10_1.h>
#include <xaudio2.h>


#include "PlayerBase.h"
#include "AudioEffect.h"
#include "Audio3D.h"
#include "VoiceCallback.h"


CZPlayer::CZPlayer(void)
{
	this->m_hThread = NULL;
	this->m_playerState.end = 0;
	this->m_playerState.mute = false;
	this->m_playerState.play = false;
	this->m_playerState.pause = false;
	this->m_playerState.start = 0;
	this->m_playerState.stop = true;
	this->m_playerState.volume = 0;
	this->m_playerControl.current = 0;
	this->m_playerControl.end = 0;
	this->m_playerControl.start = 0;
	this->m_decoder = NULL;

	memset(&this->m_playerState,0,sizeof(PLAYER_STATE));
	memset(&this->m_playerControl,9,sizeof(PLAYER_CONTROL));

	m_callbakPara.client = NULL;
	m_callbakPara.m_zCallback = NULL;
	m_audioState = (AUDIO_STATE*)malloc(sizeof(AUDIO_STATE));
}


CZPlayer::~CZPlayer(void)
{
	if(m_playerState.play || m_playerState.pause)
		Stop();
	CleanupAudio();
	//if(m_decoder)
	//{
	//	m_decoder->Close();
	//	delete m_decoder;
	//}
	free(m_audioState);
}

int CZPlayer::InitAudio()
{
	ZeroMemory(m_audioState,sizeof(AUDIO_STATE));
	CoInitializeEx(NULL,COINIT_MULTITHREADED);
	UINT32 flags = 0;
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	HRESULT hr;
	
	if(FAILED(hr = XAudio2Create(&m_audioState->pXAudio2,flags)))
		return -1;

	if( FAILED( hr = m_audioState->pXAudio2->CreateMasteringVoice( &m_audioState->pMasteringVoice ) ) )
    {
        SAFE_RELEASE( m_audioState->pXAudio2 );
        return -1;
    }

	if( FAILED( hr = m_audioState->pXAudio2->GetDeviceDetails( 0, &m_details ) ) )
    {
        SAFE_RELEASE( m_audioState->pXAudio2 );
        return -1;
    }



    //
    // Create reverb effect
    //
    flags = 0;
#ifdef _DEBUG
    flags |= XAUDIO2FX_DEBUG;
#endif

    if( FAILED( hr = XAudio2CreateReverb( &m_audioState->pReverbEffect, flags ) ) )
    {
        SAFE_RELEASE( m_audioState->pXAudio2 );
        return -1;
    }

    
     //Create a submix voice
    

    //Performance tip: you need not run global FX with the sample number
    //of channels as the final mix.  For example, this sample runs
    //the reverb in mono mode, thus reducing CPU overhead.
    XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { m_audioState->pReverbEffect, TRUE, 1 } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

    if( FAILED( hr = m_audioState->pXAudio2->CreateSubmixVoice( &m_audioState->pSubmixVoice, 1,
                                                               m_details.OutputFormat.Format.nSamplesPerSec, 0, 0,
                                                               NULL, &effectChain ) ) )
    {
        SAFE_RELEASE( m_audioState->pXAudio2 );
        SAFE_RELEASE( m_audioState->pReverbEffect );
        return -1;
    }

	XAUDIO2FX_REVERB_PARAMETERS native;
    ReverbConvertI3DL2ToNative( &g_PRESET_PARAMS[1], &native );
    m_audioState->pSubmixVoice->SetEffectParameters( 0, &native, sizeof( native ) );
	m_audioState->pSourceVoice = 0;
	// done
	m_audioState->bInitialized = true;
	m_playerState.stop = true;
	return 0;
}

void CZPlayer::CleanupAudio()
{
	if(!m_audioState->bInitialized)
		return;

	if(m_audioState->pSourceVoice)
	{
		m_audioState->pSourceVoice->DestroyVoice();
		m_audioState->pSourceVoice = NULL;
	}

	if(m_audioState->pSubmixVoice)
	{
		m_audioState->pSubmixVoice->DestroyVoice();
		m_audioState->pSubmixVoice = NULL;
	}

	if(m_audioState->pMasteringVoice)
	{
		m_audioState->pMasteringVoice->DestroyVoice();
		m_audioState->pMasteringVoice = NULL;
	}

	//m_audioState.pXAudio2->StopEngine();

	SAFE_RELEASE(m_audioState->pReverbEffect);
	SAFE_RELEASE( m_audioState->pXAudio2 );
	CoUninitialize();
	m_audioState->bInitialized = false;
}

int CZPlayer::Open(CDecoder *decoder)
{
	if(NULL != decoder) // if decoder not null,player treat it as a decoder which has opened an audio file successfully
	{
		//if(this->m_decoder)
		//{
		//	delete m_decoder;
		//}
		this->m_decoder = decoder;
	}
	else
		return -1;// not a valid decoder
	
	// stop the player
	if(this->m_playerState.play)
		this->Stop();

	// reset player control information
	this->m_playerState.start = 0;
	this->m_playerState.end = decoder->GetWaveInfo()->durationTime;

	this->m_playerControl.current = 0;
	this->m_playerControl.start = 0;
	this->m_playerControl.end = decoder->GetWaveInfo()->waveSize;
	
	return 0;
}

int CZPlayer::Pause()
{
	if(!m_audioState->bInitialized || !m_playerState.play)
		return -2;

	if(m_playerState.pause)
		return -1;

	m_audioState->pXAudio2->StopEngine();
	m_playerState.pause = true;
	if(m_callbakPara.m_zCallback)
		this->m_callbakPara.m_zCallback(this,ZPAUSE,this->m_callbakPara.client,0,0);
	return 0;
}

int CZPlayer::Play()
{
	if(m_playerState.play)
		return -1;
	if(m_hThread)
		CloseHandle(m_hThread);

	m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)PlayThread,(void *)this,0,&m_hThreadID);
	if(NULL == m_hThread)
	{
		return -3;
	}
	m_playerState.play = true;
	m_playerState.stop = false;
	return 0;
}

int CZPlayer::Play(float start,float end)
{
	if(m_playerState.play)		
		Stop();
	float durationTime = m_decoder->GetWaveInfo()->durationTime;
	if((start < 0 || start >= durationTime)
		|| (start >= end) || (end < 0 || end > durationTime))
		return -2;
	m_playerControl.current = m_playerControl.start = m_decoder->CalcaulateWaveSize(start);
	m_playerControl.end = m_decoder->CalcaulateWaveSize(end);
	m_playerState.end = end;
	m_playerState.start = start;
	Seek(start);
	return Play();
}

int CZPlayer::Stop()
{
	if(!m_audioState->bInitialized || m_playerState.stop)
		return 0;

	if(m_playerState.pause)
	{
		m_audioState->pSourceVoice->SetVolume(0);
		m_audioState->pXAudio2->StartEngine();
	}
	m_playerState.pause = false;
	m_playerState.play = false;
	WaitForSingleObject(m_hThread,INFINITE);
	if(m_hThread)
		CloseHandle(m_hThread);
	m_hThread = 0;

	m_playerState.stop = true;
	m_decoder->ResetFile();
	return 0;
}

int CZPlayer::Resume()
{
	if(!m_audioState->bInitialized)
		return -2;// the audio engine is not initialized
	if(!m_playerState.pause)
		return -1; // not paused

	m_audioState->pXAudio2->StartEngine();

	if(NULL != m_callbakPara.m_zCallback)
		m_callbakPara.m_zCallback(this,ZRESUM,m_callbakPara.client,0,0);
	m_playerState.pause = false;
	return 0;
}

int CZPlayer::Seek(float duration)
{
	if(duration < m_playerState.start || duration > m_playerState.end)
		return -2;
	if(m_playerState.play)
		Stop();
	if(FAILED(m_decoder->Seek((DWORD)duration,&m_playerControl.current)))
		return -1;
	Play();
	return 0;
}

float CZPlayer::GetPlayDuration(bool v)
{
	if(v)//return the current play section duration
	{
		return m_decoder->CalcalateTime(m_playerControl.current) 
			- m_decoder->CalcalateTime(m_playerControl.start);
	}
	else//return the total duration
	{
		return m_decoder->CalcalateTime(m_playerControl.current);
	}
}

int CZPlayer::SetCallback(ZCallbackFunction zcallback,void *client,void * /* = NULL */,void * /* = NULL */)
{
	this->m_callbakPara.client = client;
	this->m_callbakPara.m_zCallback = zcallback;
	return 0;
}

int CZPlayer::GetPlayerState(PLAYER_STATE *ps)
{
	if(NULL == ps)
		return -1;

	memset(ps,0,sizeof(PLAYER_STATE));
	
	// copy state
	ps->end = m_playerState.end;
	ps->mute = m_playerState.mute;
	ps->pause = m_playerState.pause;
	ps->play = m_playerState.play;
	ps->start = m_playerState.start;
	ps->stop = m_playerState.stop;
	ps->volume = m_playerState.volume;
	return 0;
}

int CZPlayer::SetVolume(int volume)
{
	volume = max(volume,MIN_VOLUME);
	volume = min(volume,MAX_VOLUME);

	if(volume == m_playerState.volume)
		return 0;
	m_playerState.volume = volume;
	if(m_playerState.play && !m_playerState.mute)
		m_audioState->pSourceVoice->SetVolume(1.0f * volume / 10000.0f);
	return 0;
}

int CZPlayer::SetMute(bool mute)
{
	if(m_playerState.mute == mute)
		return 0;

	m_playerState.mute = mute;
	m_audioState->pSourceVoice->SetVolume(mute?0:1.0f * m_playerState.volume / 10000.0f);
	return 0;
}

int WINAPI CZPlayer::PlayThread(void* lpdwParam)
{
	CZPlayer *ljm = (CZPlayer *)lpdwParam;

	if(NULL == ljm)
		return -10;// 
	if(ljm->m_audioState->pSourceVoice)
	{
		ljm->m_audioState->pSourceVoice->Stop(0);
		ljm->m_audioState->pSourceVoice->DestroyVoice();
		ljm->m_audioState->pSourceVoice = NULL;
	}

	WAVEFORMATEX pwfx;
	memset(&pwfx,0,sizeof(WAVEFORMATEX));
	WAVEINFO waveinfo = *ljm->m_decoder->GetWaveInfo();
	pwfx.cbSize = waveinfo.cbSize;
	pwfx.nAvgBytesPerSec = waveinfo.nAvgBytesPerSec;
	pwfx.nBlockAlign = waveinfo.nBlockAlign;
	pwfx.nChannels = waveinfo.nChannels;
	pwfx.nSamplesPerSec = waveinfo.nSamplesPerSec;
	pwfx.wBitsPerSample = waveinfo.wBitsPerSample;
	pwfx.wFormatTag = waveinfo.wFormatTag;
	HRESULT hr;
	VoiceCallback voiceCallback;
	if(FAILED(hr = ljm->m_audioState->pXAudio2->CreateSourceVoice(
		&ljm->m_audioState->pSourceVoice,&pwfx,0,XAUDIO2_DEFAULT_FREQ_RATIO,
		&voiceCallback,NULL,NULL)))
	{
		wprintf( L"Error %#X creating source voice\n", hr );
		return -9;
	}

	int CurrentDiskBuffer = 0;
	ljm->m_audioState->pSourceVoice->SetVolume(
		ljm->m_playerState.mute?0:1.0f * ljm->m_playerState.volume / 10000.0f);

	hr = ljm->m_audioState->pSourceVoice->Start(0);
	if(NULL != ljm->m_callbakPara.m_zCallback)
		ljm->m_callbakPara.m_zCallback(ljm,ZPLAY,ljm->m_callbakPara.client,0,0);

	DWORD dwRead;
	DWORD cbValid;
	DWORD has;
	
	while(ljm->m_playerControl.current < ljm->m_playerControl.end && ljm->m_playerState.play)
	{
		
		ljm->m_decoder->Read(buffers[CurrentDiskBuffer],STREAMING_BUFFER_SIZE,&dwRead);
		cbValid = min(STREAMING_BUFFER_SIZE,dwRead);
		has = ljm->m_playerControl.end - ljm->m_playerControl.current;
		if(!has)
			cbValid += has;
		else
			cbValid = min(has,dwRead);

		ljm->m_playerControl.current += cbValid;
		if(!cbValid)
		{
			cbValid = pwfx.nBlockAlign;
			ljm->m_playerControl.current = ljm->m_playerControl.end;
			memset(buffers[CurrentDiskBuffer],0,STREAMING_BUFFER_SIZE);
		}
		/*if(0 != cbValid % pwfx.nBlockAlign)
		{
			int n = pwfx.nBlockAlign - cbValid % pwfx.nBlockAlign;
			while(n > 0)
			{
				n--;
				buffers[CurrentDiskBuffer][cbValid++] = 0;
			}

		}*/
		XAUDIO2_VOICE_STATE state;
		while(ljm->m_audioState->pSourceVoice->GetState(&state),state.BuffersQueued >= MAX_BUFFER_COUNT - 1)
		{
			if(!ljm->m_playerState.play)
			{
				ljm->m_audioState->pSourceVoice->Stop(0);
				ljm->m_playerState.stop = true;
				if(NULL != ljm->m_callbakPara.m_zCallback)
					ljm->m_callbakPara.m_zCallback(ljm,ZUSERSTOP,ljm->m_callbakPara.client,0,0);
				return -1;
			}
			WaitForSingleObject( voiceCallback.hBufferEndEvent, INFINITE );	
		}

		XAUDIO2_BUFFER buf = {0};
		buf.AudioBytes = cbValid;
		buf.pAudioData = buffers[CurrentDiskBuffer];
		if( ljm->m_playerControl.current >=  ljm->m_playerControl.end )
		{
			buf.Flags = XAUDIO2_END_OF_STREAM;
		}
		//HRESULT hh = GetLastError();
		
		hr = ljm->m_audioState->pSourceVoice->SubmitSourceBuffer( &buf );
		CurrentDiskBuffer++;
		CurrentDiskBuffer %= MAX_BUFFER_COUNT;

	}

	if(NULL != ljm->m_callbakPara.m_zCallback )
	{
		if(ljm->m_playerState.play)
			ljm->m_callbakPara.m_zCallback(ljm,ZSTOP,ljm->m_callbakPara.client,0,0);
		else
			ljm->m_callbakPara.m_zCallback(ljm,ZUSERSTOP,ljm->m_callbakPara.client,0,0);
	}
	//printf("stop 1\n");
	ljm->m_audioState->pSourceVoice->Stop(0);// exception 0xC0000005
	
	ljm->m_decoder->ResetFile();
	
	//printf("stop 3\n");
	ljm->m_playerState.play = false;
	ljm->m_playerState.stop = true;
	return 0;
}