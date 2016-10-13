//  [10/13/2016 Dawn]
//	dawn.chli@gmail.com

#include "AssistantImage.h"
#include "FreeImage.h"
#include <memory.h>

    CAssistantImage::CAssistantImage()
    :m_uiWidth(0),
     m_uiHeight(0),
     m_uiPixelSize(1),
     m_uiBpp(24),
     m_uiSize(0),
     m_pBitmap(NULL)
    {
        
    }

    CAssistantImage::CAssistantImage(unsigned width,unsigned height,int bpp)
    :m_uiWidth(width),
     m_uiHeight(height),
     m_uiBpp(bpp),
     m_uiPixelSize(bpp/8),
     m_uiSize(m_uiWidth*m_uiHeight*m_uiPixelSize),
     m_pBitmap(NULL)
    {
        unsigned char* imageBuffer = new unsigned char[m_uiSize];
        memset(imageBuffer,0,m_uiSize);
        
        m_pBitmap = FreeImage_ConvertFromRawBits(imageBuffer,m_uiWidth,m_uiHeight,
                    m_uiPixelSize*m_uiWidth,bpp,FI_RGBA_BLUE_MASK,
                    FI_RGBA_GREEN_MASK,FI_RGBA_RGB_MASK);
        delete[] imageBuffer;
    }

    CAssistantImage::CAssistantImage( const char* fileName )
    :m_uiWidth(0),
     m_uiHeight(0),
     m_uiPixelSize(1),
     m_uiBpp(24),
     m_uiSize(0),
     m_pBitmap(NULL)
    {
        Load(fileName);
    }
    
    CAssistantImage::~CAssistantImage()
    {
        if( m_pBitmap != NULL )
        {
            FreeImage_Unload(m_pBitmap);
        }
    }
    
    bool CAssistantImage::Load( const char* fileName )
    {
        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
        // 从文件中获得文件类型
        fif = FreeImage_GetFileType( fileName );
        
        // 从文件名中获得文件类型
        if( fif == FIT_UNKNOWN )
        {
            fif = FreeImage_GetFIFFromFilename( fileName );
        }
        // 如果仍然无法获得图片类型则退出
        if( fif == FIT_UNKNOWN )
        {
            return false;
        }

        // 载入图像数据
        m_pBitmap = FreeImage_Load( fif,fileName );
        if( m_pBitmap == NULL )
        {
            return false;
        }
        
        m_uiWidth       = FreeImage_GetWidth(m_pBitmap);
        m_uiHeight      = FreeImage_GetHeight(m_pBitmap);
        m_uiPixelSize   = FreeImage_GetLine(m_pBitmap)/m_uiWidth;
        m_uiBpp         = m_uiPixelSize*8;
        m_uiSize        = m_uiWidth*m_uiHeight*m_uiPixelSize;
        
        return true;
    }
    
    bool CAssistantImage::Save( const char* fileName,int flag )
    {
        // TODO:这里仅支持了24及32位的图像，其他位的未做处理，后期存为灰度的图，估计会出问题
        // 指定存储为flag位的PNG图片
        FREE_IMAGE_FORMAT fif = FIF_PNG;
        
        if( flag == 24 )
        {
            if( m_uiBpp == 24 )
            {
                FreeImage_Save( fif,m_pBitmap,fileName );
            }
            else if( m_uiBpp == 32 )
            {
                // 不知FreeImage是否有对应的函数做这件事，不能就自己写
                // 将32转为24
                FIBITMAP*    pNewBitmap;
                unsigned     imageSize = m_uiWidth*m_uiHeight*3;
                unsigned char* imageBuffer = new unsigned char[imageSize];
                memset(imageBuffer,0,imageSize);
                
                pNewBitmap = FreeImage_ConvertFromRawBits(imageBuffer,m_uiWidth,m_uiHeight,
                             3*m_uiWidth,24,FI_RGBA_BLUE_MASK,FI_RGBA_GREEN_MASK,FI_RGBA_RGB_MASK);
                delete[] imageBuffer;
                
                if( pNewBitmap == NULL )
                {
                    return false;
                }
                
                _ChangeBppFrom32To24( pNewBitmap );

                FreeImage_Save( fif,pNewBitmap,fileName );

                FreeImage_Unload( pNewBitmap );
            }
        }
        else if( flag == 32 )
        {
            if( m_uiBpp == 32 )
            {
                FreeImage_Save( fif,m_pBitmap,fileName );
            }
            else if( m_uiBpp == 24 )
            {
                // TODO
                // _ChangeBppFrom24To32();
            } 
        }
		else if( m_uiBpp == 8 )
		{
			FreeImage_Save( fif,m_pBitmap,fileName,PNG_DEFAULT );
		}
        
        return true;
    }
    
    bool CAssistantImage::Draw( CAssistantImage* imageSrc,unsigned left,unsigned top )
    {
        // TODO: 如果目标图的位数与来图位数不同，可能会遇到麻烦
        // 例如: 往8位的图里画32位的东西，往24位图里画32位东西
        if( m_pBitmap == NULL )
        {
            return false;
        }
        int  resultInt = FreeImage_Paste( m_pBitmap,imageSrc->GetPFiBitmap(),left,top,255 );
        bool resultBool= (resultInt != 0) ? true : false;
        return resultBool;
    }

    void CAssistantImage::_ChangeBppFrom32To24( FIBITMAP* pNewBtm )
    {
        for( unsigned int y=0; y < m_uiHeight ;y++ )
        {   
            unsigned char* bits = FreeImage_GetScanLine( m_pBitmap,m_uiHeight - y -1 );
            unsigned char* newBits = FreeImage_GetScanLine( pNewBtm,m_uiHeight - y -1 );
            
            for( unsigned int x=0; x<m_uiWidth ;x++ )
            {
                // 这里需要乘以Alpha，否则最后图像不正确
                float alpha = bits[ x*4+3 ]/256.0f;
                newBits[ x*3+0 ] = (unsigned char)(bits[ x*4+0 ]*alpha);
                newBits[ x*3+1 ] = (unsigned char)(bits[ x*4+1 ]*alpha);
                newBits[ x*3+2 ] = (unsigned char)(bits[ x*4+2 ]*alpha);
            }
        }
    }
