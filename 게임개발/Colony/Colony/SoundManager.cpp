#include "SoundManager.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}


bool SoundManager::LoadWaveFile(const char* filename, WAVEFORMATEX& wfx, BYTE** data, DWORD& dataSize) {
    std::ifstream file(filename, std::ios::binary); //wave 형식 파일은 binary 로 취급함.
    if (!file) {
        return false;
    }

    // WAVE 파일 헤더 읽기
    char chunkID[4];
    file.read(chunkID, 4);
    if (strncmp(chunkID, "RIFF", 4) != 0) {
        return false;
    }

    file.seekg(8, std::ios::cur); // 파일 크기 건너뛰기
    //file.read(chunkID, 4);
 /*   if (strncmp(chunkID, "WAVE", 4) != 0) {
        return false;
    }*/

    // fmt 서브청크 찾기
    file.read(chunkID, 4);
    DWORD chunkSize;
    file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
    if (strncmp(chunkID, "fmt ", 4) != 0) {
        return false;
    }

    // WAVEFORMATEX 읽기
    file.read(reinterpret_cast<char*>(&wfx), sizeof(WAVEFORMATEX));
    file.seekg(chunkSize - sizeof(WAVEFORMATEX), std::ios::cur);

    // data 서브청크 찾기
    file.read(chunkID, 4);
    file.read(reinterpret_cast<char*>(&dataSize), sizeof(DWORD));
    if (strncmp(chunkID, "data", 4) != 0) {
        return false;
    }

    // 오디오 데이터 읽기
    *data = new BYTE[dataSize];
    file.read(reinterpret_cast<char*>(*data), dataSize);

    return true;
}



bool SoundManager::CreateSound()
{

    // XAudio2 초기화
    IXAudio2* pXAudio2 = nullptr;
    IXAudio2MasteringVoice* pMasterVoice = nullptr;

    HRESULT hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        // 오류 처리
        return 0;
    }

    hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
    if (FAILED(hr)) {
        // 오류 처리
        pXAudio2->Release();
        return 0;
    }

    // WAVE 파일 로드
    WAVEFORMATEX wfx = { 0 };
    BYTE* pDataBuffer = nullptr;
    DWORD dataBufferSize = 0;

    if (!LoadWaveFile("TestFile.wav", wfx, &pDataBuffer, dataBufferSize)) {
        // 오류 처리
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    // 소스 보이스 생성
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx);
    if (FAILED(hr)) {
        // 오류 처리
        delete[] pDataBuffer;
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    // 버퍼 설정
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = dataBufferSize;
    buffer.pAudioData = pDataBuffer;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    hr = pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        // 오류 처리
        pSourceVoice->DestroyVoice();
        delete[] pDataBuffer;
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    // 사운드 재생
    hr = pSourceVoice->Start(0);
    if (FAILED(hr)) {
        // 오류 처리
        pSourceVoice->DestroyVoice();
        delete[] pDataBuffer;
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    //// 사운드 재생 대기
    ////while (pSourceVoice->GetState().BuffersQueued > 0) {
    //Sleep(1000);
    ////}

    //// 리소스 정리
    //pSourceVoice->DestroyVoice();
    //delete[] pDataBuffer;
    //pMasterVoice->DestroyVoice();
    //pXAudio2->Release();

        
	
}
