package com.example.sensdataudp;

import android.os.Environment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class CFileStream
{

	//private
	private File OutFile;
	private FileOutputStream fOutStream;
	private Queue<ArrayStream> mDataQueue = null;
	private volatile boolean m_bThreadRunning = false;
	private Thread mFWriteThread=null;
	CFileStream()
	{
		mDataQueue=new ConcurrentLinkedQueue<ArrayStream>();
	}


	public boolean StartStream()
	{
		if (MainActivity.m_PermissionWrite)
		{
			File DataDir = new File(Environment.getExternalStorageDirectory(), "SensDataUdp");
			boolean dd1 = DataDir.mkdirs();
			boolean dd2 = DataDir.mkdir();

			final DateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd-HH_mm_ss");
			OutFile = new File(DataDir.getAbsolutePath(), dateFormat.format(new Date(System.currentTimeMillis())) + ".bin");
			//OutFile.mkdirs();
			try
			{
				OutFile.createNewFile();
				fOutStream = new FileOutputStream(OutFile);
			}
			catch (java.io.IOException e)
			{
				fOutStream = null;
				OutFile = null;
			}
		}

		if (fOutStream == null)
		{
			return false;
		}
		mFWriteThread=new Thread(new CFileWriter());
		if(mFWriteThread!=null)
		{
			m_bThreadRunning=true;
			mFWriteThread.start();
			return  true;
		}
		return false;
	}

	public void StopStream()
	{
		if (MainActivity.m_PermissionWrite)
		{
			m_bThreadRunning = false;
			if (mFWriteThread != null&&mFWriteThread.isAlive())
			{
				mFWriteThread.interrupt();
				try
				{
					mFWriteThread.join();
				}
				catch (java.lang.InterruptedException e)
				{

				}
			}

			try
			{
				fOutStream.close();
			}
			catch (IOException e)
			{
				fOutStream = null;
			}
			OutFile = null;
		}
	}


	public void SendPacket(ArrayStream DataPacket)
	{
		if (MainActivity.m_PermissionWrite)
		{
			mDataQueue.add(DataPacket);
		}
	}

	class CFileWriter implements Runnable
	{

		@Override
		public void run()
		{

			while (!Thread.currentThread().isInterrupted()&&m_bThreadRunning)
			{
				try
				{
					if(!mDataQueue.isEmpty())
					{
						fOutStream.write(mDataQueue.poll().toByteArray());
					}
					Thread.sleep(0,500000);
				}
				catch (IOException e)
				{
					// fOutStream = null;
				}
				catch (InterruptedException ex)
				{
					Thread.currentThread().interrupt();
				}

			}
		}
	}


}