#include "OsgModel.h"

#include <osg/ShapeDrawable>
#include <osg/Shape>

namespace scone
{
	OsgModel::OsgModel( const sim::Model& m ) : model( m )
	{
		root = new osg::Group();
		body = new osg::Geode();

		body->addDrawable( new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( 0.0f, 0.0f, 0.0f ), 0.5 ) ) );
		root->addChild( body );
	}

	void OsgModel::Update()
	{
	}

}
