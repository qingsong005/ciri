#ifndef __ciri_gfx_vertex_usage__
#define __ciri_gfx_vertex_usage__

namespace ciri {
	struct VertexUsage {
		/**
		 * Defines how a VertexElement is used.
		 */
		enum Usage {
			Position, /**< Treat as position. */
			Color,    /**< Treat as color. */
			Texcoord, /**< Treat as texcoord. */
			Normal,   /**< Treat as normal. */
			Binormal, /**< Treat as binormal. */
			Tangent   /**< Treat as tangent. */
		};
	};
} // ciri

#endif /* __ciri_gfx_vertex_usage__ */