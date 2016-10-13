//  [10/13/2016 Dawn]
//	dawn.chli@gmail.com

#ifndef AssistantImage_h__
#define AssistantImage_h__

// 前置声明
struct FIBITMAP;

class CAssistantImage
{
public:
    // 构造函数
    CAssistantImage();
    CAssistantImage( const char* fileName );
    CAssistantImage( unsigned width,unsigned height,int bpp );
    
    // 析构函数
    ~CAssistantImage();
    
    // 载入图像函数，支持FreeImage支持的格式
    bool Load( const char* fileName );

    // 暂时只支持24位和32位
    bool Save( const char* fileName,int flag = 24 );

    // 在本对象图像上绘制另一个图像
    bool Draw( CAssistantImage* imageSrc,unsigned left,unsigned top );
    
    // 获得图像参数
    unsigned GetWidth(){ return m_uiWidth; }
    unsigned GetHeight(){ return m_uiHeight; }
    unsigned GetPixelSize(){ return m_uiPixelSize; }
    unsigned GetBpp(){ return m_uiBpp; }
    unsigned GetSize(){ return m_uiSize; }
    
    // 获得FreeImage图像对象的句柄
    FIBITMAP* GetPFiBitmap(){ return m_pBitmap; }
    
private:
    
    // 将本对象的FIBITMAP从32位转为24位，24位数据保存于pNewBtm中
    void _ChangeBppFrom32To24( FIBITMAP* pNewBtm );
    
    // 图像参数
    unsigned m_uiWidth;
    unsigned m_uiHeight;
    unsigned m_uiPixelSize;
    unsigned m_uiBpp;
    unsigned m_uiSize;
    
    // FreeImage图像对象句柄
    FIBITMAP*    m_pBitmap;
};


#endif