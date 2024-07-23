
#include "SoundManager.h"


SoundManager::SoundManager()
{

}

SoundManager::~SoundManager()
{
}

bool SoundManager::InitDirectSound(HWND hWnd)
{
    // DirectSound ��ü ����
    if (FAILED(DirectSoundCreate8(nullptr, &g_pDS, nullptr))) {
        std::cerr << "DirectSound ��ü ���� ����" << std::endl;
        return false;
    }

    // ���� ���� ����
    if (FAILED(g_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY))) {
        std::cerr << "���� ���� ���� ����" << std::endl;
        return false;
    }

    // �����̸Ӹ� ���� ����
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    if (FAILED(g_pDS->CreateSoundBuffer(&dsbd, &g_pPrimaryBuffer, nullptr))) {
        std::cerr << "�����̸Ӹ� ���� ���� ����" << std::endl;
        return false;
    }

    // �����̸Ӹ� ������ ���� ����
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
   


    if (FAILED(g_pPrimaryBuffer->SetFormat(&wfx))) {
        std::cerr << "�����̸Ӹ� ���� ���� ���� ����" << std::endl;
        return false;
    }


    m_SoundVec.reserve(1000);

    return true;
}


//bool SoundManager::LoadWaveFile(const char* filename, WAVEFORMATEX& wfx, BYTE** data, DWORD& dataSize) {
//    std::ifstream file(filename, std::ios::binary); //wave ���� ������ binary �� �����.
//    if (!file) {
//        return false;
//    }
//
//    // WAVE ���� ��� �б�
//    char chunkID[4];
//    file.read(chunkID, 4);
//    if (strncmp(chunkID, "RIFF", 4) != 0) {
//        return false;
//    }
//
//    file.seekg(8, std::ios::cur); // ���� ũ�� �ǳʶٱ�
//    //file.read(chunkID, 4);
// /*   if (strncmp(chunkID, "WAVE", 4) != 0) {
//        return false;
//    }*/
//
//    // fmt ����ûũ ã��
//    file.read(chunkID, 4);
//    DWORD chunkSize;
//    file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
//    if (strncmp(chunkID, "fmt ", 4) != 0) {
//        return false;
//    }
//
//    // WAVEFORMATEX �б�
//    file.read(reinterpret_cast<char*>(&wfx), sizeof(WAVEFORMATEX));
//    file.seekg(chunkSize - sizeof(WAVEFORMATEX), std::ios::cur);
//
//    // data ����ûũ ã��
//    file.read(chunkID, 4);
//    file.read(reinterpret_cast<char*>(&dataSize), sizeof(DWORD));
//    if (strncmp(chunkID, "data", 4) != 0) {
//        return false;
//    }
//
//    // ����� ������ �б�
//    *data = new BYTE[dataSize];
//    file.read(reinterpret_cast<char*>(*data), dataSize);
//
//    return true;
//}

LPDIRECTSOUNDBUFFER SoundManager::LoadWaveToBuffer(const char* fileName) {
    // WAV ������ �޸𸮿� �ε�
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "WAV ���� ���� ����" << std::endl;
        return nullptr;
    }

    // WAV ���� ��� �б�
    char chunkId[4];
    DWORD chunkSize;
    char format[4];

    file.read(chunkId, 4);
    file.read(reinterpret_cast<char*>(&chunkSize), 4);
    file.read(format, 4);

    if (strncmp(chunkId, "RIFF", 4) != 0 || strncmp(format, "WAVE", 4) != 0) {
        std::cerr << "��ȿ���� ���� WAV ����" << std::endl;
        return nullptr;
    }

    // fmt ���� ûũ �б�
    char subChunk1Id[4];
    DWORD subChunk1Size;
    WAVEFORMATEX waveFormat;

    file.read(subChunk1Id, 4);
    file.read(reinterpret_cast<char*>(&subChunk1Size), 4);
    file.read(reinterpret_cast<char*>(&waveFormat), sizeof(WAVEFORMATEX));
    file.seekg(subChunk1Size - sizeof(WAVEFORMATEX), std::ios::cur);

    // data ���� ûũ ã��
    char subChunk2Id[4];
    DWORD subChunk2Size;

    file.read(subChunk2Id, 4);
    file.read(reinterpret_cast<char*>(&subChunk2Size), 4);

    // WAV ������ �б�
    BYTE* waveData = new BYTE[subChunk2Size];
    file.read(reinterpret_cast<char*>(waveData), subChunk2Size);

    // �������� ���� ����
    DSBUFFERDESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.dwSize = sizeof(bufferDesc);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY;
    bufferDesc.dwBufferBytes = subChunk2Size;
    bufferDesc.lpwfxFormat = &waveFormat;

    LPDIRECTSOUNDBUFFER pDSBuffer;
    if (FAILED(g_pDS->CreateSoundBuffer(&bufferDesc, &pDSBuffer, nullptr))) {
        std::cerr << "�������� ���� ���� ����" << std::endl;
        delete[] waveData;
        return nullptr;
    }

    // ���ۿ� ������ ����
    void* pBuffer = nullptr;
    DWORD bufferBytes;


    if (FAILED(pDSBuffer->Lock(0, subChunk2Size, &pBuffer, &bufferBytes, nullptr, nullptr, 0))) {
        std::cerr << "���� ��� ����" << std::endl;
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
        std::cerr << "���� ���� Ȯ�� ����" << std::endl;
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


