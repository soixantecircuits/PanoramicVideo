#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/app/KeyEvent.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class panoramicVideoApp : public AppBasic {
public:
	void setup();
    
	void keyDown( KeyEvent event );
    void keyUp( KeyEvent event );

	void fileDrop( FileDropEvent event );
    
	void update();
	void draw();
    
	void loadMovieFile( const string &path );
    
	gl::Texture					mFrameTexture, mInfoTexture;
	qtime::MovieGl				mMovie;
    Rectf                       mMovieRect;
    Vec2f                       mMove;
    bool                        mbGoLeft, mbGoRight;
    int                         dx;
};

void panoramicVideoApp::setup()
{

	//string moviePath = getOpenFilePath();
    string moviePath = "/Users/emmanuel/Documents/Soixante/projets/Bresil/realisation/contenu/Videos Bresil/Amazonia3.f4v";
	if( ! moviePath.empty() )
		loadMovieFile( moviePath );
    //cout << "Movie path:" << moviePath << endl;
    mMove = Vec2f(-2048/2+getWindowWidth()/2, -1024/2+getWindowHeight()/2);
    mbGoLeft = false;
    mbGoRight = false;
    dx = 3;
}

void panoramicVideoApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'o' ) {
		string moviePath = getOpenFilePath();
		if( ! moviePath.empty() )
			loadMovieFile( moviePath );
	}
    else if ( event.getCode() == KeyEvent::KEY_LEFT){
        //mMove += Vec2f(30, 0);
        mbGoLeft = true;
    }
    else if ( event.getCode() == KeyEvent::KEY_RIGHT){
        //mMove += Vec2f(-30, 0);
        mbGoRight = true;
    }
}

void panoramicVideoApp::keyUp( KeyEvent event )
{
    if ( event.getCode() == KeyEvent::KEY_LEFT){
        //mMove += Vec2f(30, 0);
        mbGoLeft = false;
    }
    else if ( event.getCode() == KeyEvent::KEY_RIGHT){
        //mMove += Vec2f(-30, 0);
        mbGoRight = false;
    }
}

void panoramicVideoApp::loadMovieFile( const string &moviePath )
{
	try {
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGl( moviePath );
		mMovie.setLoop();
        mMovie.setVolume(0);
		mMovie.play();
		
		// create a texture for showing some info about the movie
		TextLayout infoText;
		infoText.clear( ColorA( 0.2f, 0.2f, 0.2f, 0.5f ) );
		infoText.setColor( Color::white() );
		infoText.addCenteredLine( getPathFileName( moviePath ) );
		infoText.addLine( toString( mMovie.getWidth() ) + " x " + toString( mMovie.getHeight() ) + " pixels" );
		infoText.addLine( toString( mMovie.getDuration() ) + " seconds" );
		infoText.addLine( toString( mMovie.getNumFrames() ) + " frames" );
		infoText.addLine( toString( mMovie.getFramerate() ) + " fps" );
		infoText.setBorder( 4, 2 );
		mInfoTexture = gl::Texture( infoText.render( true ) );
	}
	catch( ... ) {
		console() << "Unable to load the movie." << std::endl;
		mMovie.reset();
		mInfoTexture.reset();
	}
    
	mFrameTexture.reset();
}

void panoramicVideoApp::fileDrop( FileDropEvent event )
{
	loadMovieFile( event.getFile( 0 ) );
}

void panoramicVideoApp::update()
{
	if( mMovie )
		mFrameTexture = mMovie.getTexture();
    if (mbGoLeft || mbGoRight){
        if (mbGoLeft) mMove += Vec2f(dx,0);
        if (mbGoRight) mMove += Vec2f(-dx,0);
        if (mMove.x > 2048) mMove.x -= 2048;
        else if (mMove.x < -2048) mMove.x += 2048;
    }
}

void panoramicVideoApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
    
	if( mFrameTexture ) {
        mMovieRect = Rectf( mFrameTexture.getBounds());
        mMovieRect.offset(mMove);
		gl::draw( mFrameTexture, mMovieRect);
        mMovieRect.offset(Vec2f(-2048,0));
        gl::draw( mFrameTexture, mMovieRect);
        mMovieRect.offset(Vec2f(2048*2,0));
        gl::draw( mFrameTexture, mMovieRect);
	}
    
	if( mInfoTexture ) {
		glDisable( GL_TEXTURE_RECTANGLE_ARB );
		gl::draw( mInfoTexture, Vec2f( 20, getWindowHeight() - 20 - mInfoTexture.getHeight() ) );
	}
}

CINDER_APP_BASIC( panoramicVideoApp, RendererGl );
