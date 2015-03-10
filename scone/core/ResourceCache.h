#pragma once

namespace scone
{
	template< typename T >
	class ResourceCache
	{
	public:
		ResourceCache() { };
		~ResourceCache() { };
		std::unique_ptr< T > CreateCopy( const String& name )
		{
			auto it = m_Resources.find( name );
			if ( it == m_Resources.end() )
			{
				T* r = CreateFirst( name );
				m_Resources[ name ] = std::unique_ptr< T >( r );
				it = m_Resources.find( name );
			}
			return std::unique_ptr< T >( new T( *it->second ) );
		}
			
	private:
		T* CreateFirst( const String& name ) { SCONE_THROW_NOT_IMPLEMENTED; }
		std::map< std::string, std::unique_ptr< T > > m_Resources;
	};
}