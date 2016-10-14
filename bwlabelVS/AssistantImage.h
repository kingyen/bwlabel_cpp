//  [10/13/2016 Dawn]
//	dawn.chli@gmail.com

#ifndef AssistantImage_h__
#define AssistantImage_h__

// ǰ������
struct FIBITMAP;

class CAssistantImage
{
public:
    // ���캯��
    CAssistantImage();
    CAssistantImage( const char* fileName );
    CAssistantImage( unsigned width,unsigned height,int bpp );
    
    // ��������
    ~CAssistantImage();
    
    // ����ͼ������֧��FreeImage֧�ֵĸ�ʽ
    bool Load( const char* fileName );

    // ��ʱֻ֧��24λ��32λ
    bool Save( const char* fileName,int flag = 24 );

    // �ڱ�����ͼ���ϻ�����һ��ͼ��
    bool Draw( CAssistantImage* imageSrc,unsigned left,unsigned top );
    
    // ���ͼ�����
    unsigned GetWidth(){ return m_uiWidth; }
    unsigned GetHeight(){ return m_uiHeight; }
    unsigned GetPixelSize(){ return m_uiPixelSize; }
    unsigned GetBpp(){ return m_uiBpp; }
    unsigned GetSize(){ return m_uiSize; }
    
    // ���FreeImageͼ�����ľ��
    FIBITMAP* GetPFiBitmap(){ return m_pBitmap; }
    
private:
    
    // ���������FIBITMAP��32λתΪ24λ��24λ���ݱ�����pNewBtm��
    void _ChangeBppFrom32To24( FIBITMAP* pNewBtm );
    
    // ͼ�����
    unsigned m_uiWidth;
    unsigned m_uiHeight;
    unsigned m_uiPixelSize;
    unsigned m_uiBpp;
    unsigned m_uiSize;
    
    // FreeImageͼ�������
    FIBITMAP*    m_pBitmap;
};


#endif