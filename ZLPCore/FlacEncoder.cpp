#include "FlacEncoder.h"


CFlacEncoder::CFlacEncoder(void)
{
	compLevel = 5;
}


CFlacEncoder::~CFlacEncoder(void)
{
}

int CFlacEncoder::Open(const wchar_t *aFile,AudioFormat af)
{
	start = 0;
	int r =  decoder->Open(aFile,af);

	end = decoder->GetWaveInfo()->durationTime;
	return r;
}

int CFlacEncoder::SetTimes(float s,float e)
{
	if(s >= 0 && s < e && e <= decoder->GetWaveInfo()->durationTime)
	{
		start = s;
		end = e;
	}
	
	return 0;
}

int CFlacEncoder::SetCompressionLevel(unsigned int compLevel)
{
	if(compLevel >=0 && compLevel <= 8)
		this->compLevel = compLevel;
	return 0;
}

int CFlacEncoder::Save(const wchar_t *fFile)
{
	DWORD curPos = 0;
	decoder->Seek(start,&curPos);

	unsigned int size = decoder->CalcaulateWaveSize(end)- decoder->CalcaulateWaveSize(start) ;
	total_samples = (double)decoder->GetWaveInfo()->waveSize/(double)(decoder->GetWaveInfo()->nChannels
		*decoder->GetWaveInfo()->wBitsPerSample/8);
	WAVEINFO waveInfo = *decoder->GetWaveInfo();
	FLAC__bool ok = true;
	FLAC__StreamEncoder *encoder = 0;
	FLAC__StreamEncoderInitStatus init_status;
	FLAC__StreamMetadata *metadata[2];
	FLAC__StreamMetadata_VorbisComment_Entry entry;
	
	if(NULL == (encoder = FLAC__stream_encoder_new()))
	{
		return 2;
	}

	ok &= FLAC__stream_encoder_set_verify(encoder, true);
	ok &= FLAC__stream_encoder_set_compression_level(encoder, compLevel);
	ok &= FLAC__stream_encoder_set_channels(encoder, waveInfo.nChannels);
	ok &= FLAC__stream_encoder_set_bits_per_sample(encoder, waveInfo.wBitsPerSample);
	ok &= FLAC__stream_encoder_set_sample_rate(encoder, waveInfo.nSamplesPerSec);
	ok &= FLAC__stream_encoder_set_total_samples_estimate(encoder, total_samples);

	if(ok) {
		if(
			(metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL ||
			(metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING)) == NULL ||
			
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", artist.GetData()) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) || 
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR", year.GetData()) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) ||
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ALBUM", album.GetData()) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) ||
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "TITLE", title.GetData()) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) ||
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "GENRE", genre.GetData())||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) ||
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "COMMENT", comment.GetData()) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) ||
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "TRACK", track.GetData()) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false)
		) {
			fprintf(stderr, "ERROR: out of memory or tag error\n");
			ok = false;
		}

		metadata[1]->length = 1234; /* set the padding length */

		ok = FLAC__stream_encoder_set_metadata(encoder, metadata, 2);
	}

	char * mfile = 0;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,fFile,-1,NULL,0,NULL,FALSE);
	mfile = new char[dwNum + 1];
	WideCharToMultiByte (CP_OEMCP,NULL,fFile,-1,mfile,dwNum,NULL,FALSE);
	mfile[dwNum] = '\0';

	if(ok) 
	{
		init_status = FLAC__stream_encoder_init_file(encoder, mfile, progress_callback, /*client_data=*/NULL);

		if(FLAC__STREAM_ENCODER_INIT_STATUS_OK != init_status)
		{
			ok = false;	
		}
	}
	

	DWORD dwRead = 0;
	DWORD nRead = 0;
	/* read blocks of samples from WAVE file and feed to encoder */
	if(ok) {
		size_t left = (size_t)total_samples;
		while(ok && left) {
			size_t need = (left>READSIZE? (size_t)READSIZE : (size_t)left);
			nRead = waveInfo.nChannels *(waveInfo.wBitsPerSample/8)*need;
			decoder->Read(buffer,nRead,&dwRead);
			if(nRead != dwRead/*fread(buffer, channels*(bps/8), need, fin) != need*/) 
			{
				//fprintf(stderr, "ERROR: reading from WAVE file\n");
				ok = false;
			}
			else {
				/* convert the packed little-endian 16-bit PCM samples from WAVE into an interleaved FLAC__int32 buffer for libFLAC */
				size_t i;
				for(i = 0; i < need*waveInfo.nChannels; i++) {
					/* inefficient but simple and works on big- or little-endian machines */
					pcm[i] = (FLAC__int32)(((FLAC__int16)(FLAC__int8)buffer[2*i+1] << 8) | (FLAC__int16)buffer[2*i]);
				}
				/* feed samples to encoder */
				ok = FLAC__stream_encoder_process_interleaved(encoder, pcm, need);
			}

			if(0 != zDProcCallback.zDProcCallbackfuc)
				zDProcCallback.zDProcCallbackfuc(this,1,size,(total_samples-left)*waveInfo.nChannels *(waveInfo.wBitsPerSample/8),zDProcCallback.client);
			left -= need;
		}
	}

	ok &= FLAC__stream_encoder_finish(encoder);

	

	/* now that encoding is finished, the metadata can be freed */
	FLAC__metadata_object_delete(metadata[0]);
	FLAC__metadata_object_delete(metadata[1]);

	FLAC__stream_encoder_delete(encoder);
	delete mfile;
	return 0;
}

void CFlacEncoder::progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written, FLAC__uint64 samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data)
{
		//fprintf(stderr, "wrote %I64u bytes, %I64u samples, %u/%u frames\n", bytes_written, samples_written, frames_written, total_frames_estimate);
}