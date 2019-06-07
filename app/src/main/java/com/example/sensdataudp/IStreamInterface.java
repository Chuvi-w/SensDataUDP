package com.example.sensdataudp;
public interface IStreamInterface
{
    boolean StartStream();

    void StopStream();

    void SendPacket(ArrayStream DataPacket);

    StringBuilder GetStat();

    boolean IsEnable();
}
