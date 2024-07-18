#include "SoundManager.h"


SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::Intialize()
{
    //initialize
    XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    pXAudio2->CreateMasteringVoice(&pMasterVoice);
}


bool SoundManager::LoadWaveFile(const char* filename, WAVEFORMATEX& wfx, BYTE** data, DWORD& dataSize) {
    std::ifstream file(filename, std::ios::binary); //wave ���� ������ binary �� �����.
    if (!file) {
        return false;
    }

    // WAVE ���� ��� �б�
    char chunkID[4];
    file.read(chunkID, 4);
    if (strncmp(chunkID, "RIFF", 4) != 0) {
        return false;
    }

    file.seekg(8, std::ios::cur); // ���� ũ�� �ǳʶٱ�
    //file.read(chunkID, 4);
 /*   if (strncmp(chunkID, "WAVE", 4) != 0) {
        return false;
    }*/

    // fmt ����ûũ ã��
    file.read(chunkID, 4);
    DWORD chunkSize;
    file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
    if (strncmp(chunkID, "fmt ", 4) != 0) {
        return false;
    }

    // WAVEFORMATEX �б�
    file.read(reinterpret_cast<char*>(&wfx), sizeof(WAVEFORMATEX));
    file.seekg(chunkSize - sizeof(WAVEFORMATEX), std::ios::cur);

    // data ����ûũ ã��
    file.read(chunkID, 4);
    file.read(reinterpret_cast<char*>(&dataSize), sizeof(DWORD));
    if (strncmp(chunkID, "data", 4) != 0) {
        return false;
    }

    // ����� ������ �б�
    *data = new BYTE[dataSize];
    file.read(reinterpret_cast<char*>(*data), dataSize);

    return true;
}

IXAudio2SourceVoice* SoundManager::AddSound(const char* filename)
{
    // WAVE ���� �ε�
    WAVEFORMATEX wfx = { 0 };
    BYTE* pDataBuffer = nullptr;
    DWORD dataBufferSize = 0;


    if (!LoadWaveFile(filename, wfx, &pDataBuffer, dataBufferSize)) {
        // ���� ó��
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    // �ҽ� ���̽� ����
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    HRESULT hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx);
    if (FAILED(hr)) {
        // ���� ó��
        delete[] pDataBuffer;
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    // ���� ����
    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = dataBufferSize;
    buffer.pAudioData = pDataBuffer;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    hr = pSourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        // ���� ó��
        pSourceVoice->DestroyVoice();
        delete[] pDataBuffer;
        pMasterVoice->DestroyVoice();
        pXAudio2->Release();
        return 0;
    }

    m_SourceVoices.push_back(pSourceVoice);
    m_buffers.push_back(pDataBuffer);

    return pSourceVoice;
}

void SoundManager::Release()
{
    for (auto& sourceVoice : m_SourceVoices) {
        sourceVoice->DestroyVoice();
    }
    for (auto& buffer : m_buffers) {
        delete[] buffer;
    }
    pMasterVoice->DestroyVoice();
    pXAudio2->Release();
}

