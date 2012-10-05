package com.util;
 
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
 
/**
 * ��ѹZIPѹ���ļ���ָ����Ŀ¼
 * @author LanP
 * @since 2012-3-13 8:57:18
 */
public final class ZipToFile {
    /**
     * ��������СĬ��20480
     */
    private final static int FILE_BUFFER_SIZE = 20480;
     
    private ZipToFile() {
         
    }
     
    /**
     * ��ָ��Ŀ¼��ZIPѹ���ļ���ѹ��ָ����Ŀ¼
     * @param zipFilePath       ZIPѹ���ļ���·��
     * @param zipFileName       ZIPѹ���ļ�����
     * @param targetFileDir     ZIPѹ���ļ�Ҫ��ѹ����Ŀ¼
     * @return flag             ��������ֵ
     */
    public static boolean unzip(String zipFilePath, String zipFileName, String targetFileDir){
        boolean flag = false;
        //1.�ж�ѹ���ļ��Ƿ���ڣ��Լ�����������Ƿ�Ϊ��
        File file = null;           //ѹ���ļ�(��·��)
        ZipFile zipFile = null;
        file = new File(zipFilePath + "/" + zipFileName);
        
       // zipFile = new ZipFile(file, "GBK");
        
        System.out.println(">>>>>>��ѹ�ļ���" + zipFilePath + "/" + zipFileName + "������" + targetFileDir + "��Ŀ¼��<<<<<<");
        if(false == file.exists()) {
            System.out.println(">>>>>>ѹ���ļ���" + zipFilePath + "/" + zipFileName + "��������<<<<<<");
            return false;
        } else if(0 == file.length()) {
            System.out.println(">>>>>>ѹ���ļ���" + zipFilePath + "/" + zipFileName + "����СΪ0����Ҫ��ѹ<<<<<<");
            return false;
        } else {
            //2.��ʼ��ѹZIPѹ���ļ��Ĵ���
            byte[] buf = new byte[FILE_BUFFER_SIZE];
            int readSize = -1;
            ZipInputStream zis = null;
            FileOutputStream fos = null;
            try {
                // ����Ƿ���zip�ļ�
                zipFile = new ZipFile(file);

                zipFile.close();
                // �ж�Ŀ��Ŀ¼�Ƿ���ڣ��������򴴽�
                File newdir = new File(targetFileDir);
                if (false == newdir.exists()) {
                    newdir.mkdirs();
                    newdir = null;
                }
                zis = new ZipInputStream(new FileInputStream(file));
                ZipEntry zipEntry = zis.getNextEntry();
                // ��ʼ��ѹ�������ļ����д���
                while (null != zipEntry) {
                    String zipEntryName = zipEntry.getName().replace('\\', '/');
                    //�ж�zipEntry�Ƿ�ΪĿ¼������ǣ��򴴽�
                    if(zipEntry.isDirectory()) {
                        int indexNumber = zipEntryName.lastIndexOf('/');
                        File entryDirs = new File(targetFileDir + "/" + zipEntryName.substring(0, indexNumber));
                        entryDirs.mkdirs();
                        entryDirs = null;
                    } else {
                        try {
                            fos = new FileOutputStream(targetFileDir + "/" + zipEntryName);
                            while ((readSize = zis.read(buf, 0, FILE_BUFFER_SIZE)) != -1) {
                                fos.write(buf, 0, readSize);
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                            throw new RuntimeException(e.getCause()); 
                        } finally {
                            try {
                                if (null != fos) {
                                    fos.close();
                                }
                            } catch (IOException e) {
                                e.printStackTrace();
                                throw new RuntimeException(e.getCause()); 
                            }
                        }
                    }
                    zipEntry = zis.getNextEntry();
                }
                flag = true;
            } catch (ZipException e) {
                e.printStackTrace();
                throw new RuntimeException(e.getCause()); 
            } catch (IOException e) {
                e.printStackTrace();
                throw new RuntimeException(e.getCause()); 
            } finally {
                try {
                    if (null != zis) {
                        zis.close();
                    }
                    if (null != fos) {
                        fos.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    throw new RuntimeException(e.getCause()); 
                }
            }
        }
        return flag;
    }
    public static boolean unzip(File file, String targetFileDir){
        boolean flag = false;
        //1.�ж�ѹ���ļ��Ƿ���ڣ��Լ�����������Ƿ�Ϊ��
                 //ѹ���ļ�(��·��)
        ZipFile zipFile = null;

        if(false == file.exists()) {
            return false;
        } else if(0 == file.length()) {
            return false;
        } else {
            //2.��ʼ��ѹZIPѹ���ļ��Ĵ���
            byte[] buf = new byte[FILE_BUFFER_SIZE];
            int readSize = -1;
            ZipInputStream zis = null;
            FileOutputStream fos = null;
            try {
                // ����Ƿ���zip�ļ�
                zipFile = new ZipFile(file);

                zipFile.close();
                // �ж�Ŀ��Ŀ¼�Ƿ���ڣ��������򴴽�
                File newdir = new File(targetFileDir);
                if (false == newdir.exists()) {
                    newdir.mkdirs();
                    newdir = null;
                }
                zis = new ZipInputStream(new FileInputStream(file));
                ZipEntry zipEntry = zis.getNextEntry();
                // ��ʼ��ѹ�������ļ����д���
                while (null != zipEntry) {
                    String zipEntryName = zipEntry.getName().replace('\\', '/');
                    //�ж�zipEntry�Ƿ�ΪĿ¼������ǣ��򴴽�
                    if(zipEntry.isDirectory()) {
                        int indexNumber = zipEntryName.lastIndexOf('/');
                        File entryDirs = new File(targetFileDir + "/" + zipEntryName.substring(0, indexNumber));
                        entryDirs.mkdirs();
                        entryDirs = null;
                    } else {
                        try {
                            fos = new FileOutputStream(targetFileDir + "/" + zipEntryName);
                            while ((readSize = zis.read(buf, 0, FILE_BUFFER_SIZE)) != -1) {
                                fos.write(buf, 0, readSize);
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                            throw new RuntimeException(e.getCause()); 
                        } finally {
                            try {
                                if (null != fos) {
                                    fos.close();
                                }
                            } catch (IOException e) {
                                e.printStackTrace();
                                throw new RuntimeException(e.getCause()); 
                            }
                        }
                    }
                    zipEntry = zis.getNextEntry();
                }
                flag = true;
            } catch (ZipException e) {
                e.printStackTrace();
                throw new RuntimeException(e.getCause()); 
            } catch (IOException e) {
                e.printStackTrace();
                throw new RuntimeException(e.getCause()); 
            } finally {
                try {
                    if (null != zis) {
                        zis.close();
                    }
                    if (null != fos) {
                        fos.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    throw new RuntimeException(e.getCause()); 
                }
            }
        }
        return flag;
    }
    /**
     * �����õ�Main����
     */
    public static void main(String[] args) {
        String zipFilePath = "d:\\tmp";
        String zipFileName = "test.zip";
        String targetFileDir = "d:\\tmp";
        
        File file = null;           //ѹ���ļ�(��·��)
        file = new File(zipFilePath + "/" + zipFileName);
        boolean flag = ZipToFile.unzip(file, targetFileDir);
        //boolean flag = ZipToFile.unzip(zipFilePath, zipFileName, targetFileDir);
        if(flag) {
            System.out.println(">>>>>>��ѹ�ɹ�<<<<<<");
        } else {
            System.out.println(">>>>>>��ѹʧ��<<<<<<");
        }
    }
 
}