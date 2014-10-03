#include "ventanaAprendizaje.h"
#include "ui_ventanaaprendizaje.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <assert.h>
#include "opencv2/core/core.hpp"

#include "CameraWidget.h"
#include "CameraWindow.h"

#include <QTextEdit>
#include <QLineEdit>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>


using namespace Qt;
using namespace std;
using namespace cv;

ventanaAprendizaje::ventanaAprendizaje(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ventanaAprendizaje)
{
    ui->setupUi(this);
    camera = cvCreateCameraCapture(0);
    cvQueryFrame(camera);
    QVBoxLayout *layout = new QVBoxLayout;

    windowCamara = new CameraWindow(camera,0);

    QWidget *widget = new QWidget(this);
    QPushButton *btn = new QPushButton("Test");

    connect(windowCamara, SIGNAL(guardeImagen()), this, SLOT(elSlot()));

    widget->resize(600,600);
    layout->addWidget(btn);
    layout->addWidget(windowCamara);
    widget->setLayout(layout);

    //###############################################################

    ui->output->setReadOnly(true);
}

void ventanaAprendizaje::procesadorImagen(string pUltimaImagen)
{


    std::string str = pUltimaImagen;
    const char *cstr = str.c_str();


    IplImage* img =  cvLoadImage(cstr);

    //show the original image
    cvNamedWindow("Original");
    cvShowImage("Original",img);

    //smooth the original image using Gaussian kernel to remove noise
    cvSmooth(img, img, CV_GAUSSIAN,3,3);

    //converting the original image into grayscale
    IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
    cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

    cvNamedWindow("GrayScale Image");
    cvShowImage("GrayScale Image",imgGrayScale);

    //thresholding the grayscale image to get better results
    cvThreshold(imgGrayScale,imgGrayScale,100,255,CV_THRESH_BINARY_INV);

    cvNamedWindow("Thresholded Image");
    cvShowImage("Thresholded Image",imgGrayScale);

    CvSeq* contour;  //hold the pointer to a contour
    CvSeq* result;   //hold sequence of points of a contour
    CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    //finding all contours in the image
    cvFindContours(imgGrayScale, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

    //iterating through each contour
    while(contour)
    {
        //obtain a sequence of points of the countour, pointed by the variable 'countour'
        result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.02, 0);

        //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
        if(result->total==3 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>100 )
        {
            //iterating through each point
            CvPoint *pt[3];
            for(int i=0;i<3;i++){
                pt[i] = (CvPoint*)cvGetSeqElem(result, i);
            }

            //drawing lines around the triangle
            cvLine(img, *pt[0], *pt[1], cvScalar(255,0,0),4);
            cvLine(img, *pt[1], *pt[2], cvScalar(255,0,0),4);
            cvLine(img, *pt[2], *pt[0], cvScalar(255,0,0),4);

        }
        if (fabs(cvContourArea(result, CV_WHOLE_SEQ))>100){
            Mat src, src_gray;

            /// Read the image
            src = imread(str, 1 );



            /// Convert it to gray
            cvtColor( src, src_gray, CV_BGR2GRAY );

            /// Reduce the noise so we avoid false circle detection
            GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

            vector<Vec3f> circles;

            /// Apply the Hough Transform to find the circles
            HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 0, 0 );

            /// Draw the circles detected
            for( size_t i = 0; i < circles.size(); i++ )
            {
                Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                int radius = cvRound(circles[i][2]);
                // circle center
                circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
                // circle outline
                circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
             }

            /// Show your results
            namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
            imshow( "Hough Circle Transform Demo", src );

        }

        //obtain the next contour
        contour = contour->h_next;
    }

    //show the image in which identified shapes are marked
    cvNamedWindow("Tracked");
    cvShowImage("Tracked",img);

    cvWaitKey(0); //wait for a key press

    //cleaning up
    cvDestroyAllWindows();
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&img);
    cvReleaseImage(&imgGrayScale);

}


void ventanaAprendizaje::elSlot()
{

    qDebug() << "Se guardo una imagen nueva";
    //Identificacion y abstraccion
    std::string ultimaImagen = windowCamara->getultimaImagen();
    std::cout << ultimaImagen << std::endl;
    procesadorImagen(ultimaImagen);

    ui->output->setTextColor("red");
    ui->output->append("Ordenador: Como deberia llamar esta forma?");
    //formas.nuevaforma(vertices, nombre)
}

void ventanaAprendizaje::on_botonEnviar_clicked()
{
    ui->output->setTextColor("blue");
    ui->output->append("Usuario: " + ui->input->text());
}

ventanaAprendizaje::~ventanaAprendizaje()
{
    delete ui;
}
