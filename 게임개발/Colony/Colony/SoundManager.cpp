
#include "SoundManager.h"


SoundManager::SoundManager()
{

}

SoundManager::~SoundManager()
{
}

bool SoundManager::InitDirectSound(HWND hWnd)
{
    // DirectSound 객체 생성
    if (FAILED(DirectSoundCreate8(nullptr, &g_pDS, nullptr))) {
        std::cerr << "DirectSound 객체 생성 실패" << std::endl;
        return false;
    }

    // 협력 수준 설정
    if (FAILED(g_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY))) {
        std::cerr << "협력 수준 설정 실패" << std::endl;
        return false;
    }

    // 프라이머리 버퍼 생성
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if (FAILED(g_pDS->CreateSoundBuffer(&dsbd, &g_pPrimaryBuffer, nullptr))) {
        std::cerr << "프라이머리 버퍼 생성 실패" << std::endl;
        return false;
    }

    // 프라이머리 버퍼의 포맷 설정
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
   


    if (FAILED(g_pPrimaryBuffer->SetFormat(&wfx))) {
        std::cerr << "프라이머리 버퍼 포맷 설정 실패" << std::endl;
        return false;
    }


    m_SoundVec.reserve(1000);

    return true;
}


//bool SoundManager::LoadWaveFile(const char* filename, WAVEFORMATEX& wfx, BYTE** data, DWORD& dataSize) {
//    std::ifstream file(filename, std::ios::binary); //wave 형식 파일은 binary 로 취급함.
//    if (!file) {
//        return false;
//    }
//
//    // WAVE 파일 헤더 읽기
//    char chunkID[4];
//    file.read(chunkID, 4);
//    if (strncmp(chunkID, "RIFF", 4) != 0) {
//        return false;
//    }
//
//    file.seekg(8, std::ios::cur); // 파일 크기 건너뛰기
//    //file.read(chunkID, 4);
// /*   if (strncmp(chunkID, "WAVE", 4) != 0) {
//        return false;
//    }*/
//
//    // fmt 서브청크 찾기
//    file.read(chunkID, 4);
//    DWORD chunkSize;
//    file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
//    if (strncmp(chunkID, "fmt ", 4) != 0) {
//        return false;
//    }
//
//    // WAVEFORMATEX 읽기
//    file.read(reinterpret_cast<char*>(&wfx), sizeof(WAVEFORMATEX));
//    file.seekg(chunkSize - sizeof(WAVEFORMATEX), std::ios::cur);
//
//    // data 서브청크 찾기
//    file.read(chunkID, 4);
//    file.read(reinterpret_cast<char*>(&dataSize), sizeof(DWORD));
//    if (strncmp(chunkID, "data", 4) != 0) {
//        return false;
//    }
//
//    // 오디오 데이터 읽기
//    *data = new BYTE[dataSize];
//    file.read(reinterpret_cast<char*>(*data), dataSize);
//
//    return true;
//}

LPDIRECTSOUNDBUFFER SoundManager::LoadWaveToBuffer(const char* fileName) {
    // WAV 파일을 메모리에 로드
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "WAV 파일 열기 실패" << std::endl;
        return nullptr;
    }

    // WAV 파일 헤더 읽기
    char chunkId[4];
    DWORD chunkSize;
    char format[4];

    file.read(chunkId, 4);
    file.read(reinterpret_cast<char*>(&chunkSize), 4);
    file.read(format, 4);

    if (strncmp(chunkId, "RIFF", 4) != 0 || strncmp(format, "WAVE", 4) != 0) {
        std::cerr << "유효하지 않은 WAV 파일" << std::endl;
        return nullptr;
    }

    // fmt 서브 청크 읽기
    char subChunk1Id[4];
    DWORD subChunk1Size;
    WAVEFORMATEX waveFormat;

    file.read(subChunk1Id, 4);
    file.read(reinterpret_cast<char*>(&subChunk1Size), 4);
    file.read(reinterpret_cast<char*>(&waveFormat), sizeof(WAVEFORMATEX));
    file.seekg(subChunk1Size - sizeof(WAVEFORMATEX), std::ios::cur);

    // data 서브 청크 찾기
    char subChunk2Id[4];
    DWORD subChunk2Size;

    file.read(subChunk2Id, 4);
    file.read(reinterpret_cast<char*>(&subChunk2Size), 4);

    // WAV 데이터 읽기
    BYTE* waveData = new BYTE[subChunk2Size];
    file.read(reinterpret_cast<char*>(waveData), subChunk2Size);

    // 세컨더리 버퍼 생성
    DSBUFFERDESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.dwSize = sizeof(bufferDesc);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY;
    bufferDesc.dwBufferBytes = subChunk2Size;
    bufferDesc.lpwfxFormat = &waveFormat;

    LPDIRECTSOUNDBUFFER pDSBuffer;
    if (FAILED(g_pDS->CreateSoundBuffer(&bufferDesc, &pDSBuffer, nullptr))) {
        std::cerr << "세컨더리 버퍼 생성 실패" << std::endl;
        delete[] waveData;
        return nullptr;
    }

    // 버퍼에 데이터 복사
    void* pBuffer = nullptr;
    DWORD bufferBytes;


    if (FAILED(pDSBuffer->Lock(0, subChunk2Size, &pBuffer, &bufferBytes, nullptr, nullptr, 0))) {
        std::cerr << "버퍼 잠금 실패" << std::endl;
        pDSBuffer->Release();
        delete[] waveData;
        return nullptr;
    }

    memcpy(pBuffer, waveData, bufferBytes);
    pDSBuffer->Unlock(pBuffer, bufferBytes, nullptr, 0);
    delete[] waveData;

    m_SoundVec.push_back(pDSBuffer);

    return pDSBuffer;
}

void SoundManager::ChangeSceneSound()
{
    for (auto& Sound : m_SoundVec) {
        if(Sound)
        Sound->Stop();
    }
}

void SoundManager::RestartSound(LPDIRECTSOUNDBUFFER sounbuffer)
{
    sounbuffer->SetCurrentPosition(0);
    sounbuffer->Play(0, 0, 0);
}

bool SoundManager::IsSoundBufferPlaying(LPDIRECTSOUNDBUFFER pDSBuffer) {
    DWORD status;
    if (FAILED(pDSBuffer->GetStatus(&status))) {
        std::cerr << "버퍼 상태 확인 실패" << std::endl;
        return false;
    }
    return (status & DSBSTATUS_PLAYING) != 0;
}



void SoundManager::Cleanup()
{
    if (g_pSecondaryBuffer) {
        g_pSecondaryBuffer->Release();
        g_pSecondaryBuffer = nullptr;
    }

    if (g_pPrimaryBuffer) {
        g_pPrimaryBuffer->Release();
        g_pPrimaryBuffer = nullptr;
    }

    if (g_pDS) {
        g_pDS->Release();
        g_pDS = nullptr;
    }

    m_SoundVec.clear();
}


