#pragma once

#include "../graphics.h"
#include "defs.hpp"
#include "TileMap.hpp"
#include "MapConfig.hpp"
#include "RoadGenerator.hpp"
#include "Voronoi.hpp"
#include "District.hpp"
#include "Skyscraper.h"
#include "../UI/Playing/UIPlaying.h"
#include "Environment.hpp"
#include "../Lights.h"

// TODO: refactor out
class Streetlight {
public:
	Streetlight( Vector3 const &worldPosition, Graphics &graphics, LightList &lights, Vector3 rotation={.0f,.0f,.0f} ): graphics(graphics), lights(lights) {
		//Vector3 lightColor    = { .8f, .9f, 1.0f };
		//F32     lightStrength = { 80.0f };
		model = std::make_unique<GameObject>();

		model->mesh       = graphics.getMeshPointer( "Entities/Streetlight"   );
		model->setScale({ .25f, .25f, .25f });
		model->setColor({ .75f, .7f, .7f, 1.0f });
		//model->setSpotShadow( false );
		graphics.addToDraw( model.get() );

		//model->mesh       = graphics.getMeshPointer( "Streetlight"   );
		//model->setMaterial( graphics.getMaterial(    "Streetlight" ) );
		model->setPosition( worldPosition+Vector3(.0f,-1.5f,.0f) );
		model->setRotation( rotation );
		//light = lights.addLight( PointLight(worldPosition+Vector3{.0f,5.0f,.0f}, {lightColor}, lightStrength) );
		// rigid body?
	}
	Streetlight(            Streetlight const  & ) = delete;
	Streetlight(            Streetlight       && ) = delete;
	Streetlight& operator=( Streetlight const  & ) = delete;
	Streetlight& operator=( Streetlight       && ) = delete;
	~Streetlight() {
		// graphics.removeFromDraw( dynamicTop );
		//lights.removeLight( light );
		graphics.removeFromDraw( model.get() );
	}

private:
	Graphics         &graphics;
	LightList        &lights;
	UPtr<GameObject>  model; // staticBase + dynamicTop?
	//PointLight       *light; // remove on collision?
};


// Tile neighbours bit layout:
//
//  7     0     1
//    NW  N  NE
//
//  6 W   c   E 2
//
//    SW  S  SE 
//  5     4     3
//
// c is not included (it's intrinsic based off of the currently processed tiled)

union NeighbourMask {
		U8 bitmap  { 0x00 }; // <- encoding byte; default unset
		struct {             // <- individual bit access interface
			U8 n  : 1,
			   ne : 1,
			   e  : 1,
			   se : 1,
			   s  : 1,
			   sw : 1,
			   w  : 1,
			   nw : 1;
		};
};


struct HouseTileset {
	String                  name;
	F32                     floorHeight;
	Vector<District::Enum>	validInDistricts;
};

extern Vector<HouseTileset> const houseTilesets;

auto getCompatibleTilesets( District::Enum d ) noexcept;

struct MultitileLayout {
   U32        width, length; // size() => width*length
   Vector<U8> floors;        // floors[y * width +x]
};

extern Vector<MultitileLayout> multitileLayouts;

void loadMultitileLayouts( String const &path );

Opt<const MultitileLayout *> getMultitileLayout( District::Enum, RNG & ) noexcept;

// struct PrefabHouse {
// 	TODO
// };

struct CompositeHouse {
	GameObject          walls, windows, roof;
	GameObject			area;
	V2u                 dimensions;
	Vector3				offset;
	String				skyscraperMeshIndex;
};

struct SingleTileHouse {
	GameObject   object;
};

struct MultiTileHouse {
	Vector<GameObject> parts;
	Vector<GameObject> hitboxes;
	Bounds             bounds; // refactor remove?
	V2u                nw;
	MultitileLayout    layout;
};

struct HouseGenData {
	Vector<SingleTileHouse>  singles;
	Vector<MultiTileHouse>   multis;
	Vector<CompositeHouse>   composites;
};

struct Border {
	Vector<GameObject>  meshes;
	Vector<GameObject>  bounds { Size(4) };
};


class Map {
public:
	// for transition screen
	struct Info {
		Environment environment;
		Size        width,length;
	};

	Map( Graphics &, MapConfig const &, Physics *, LightList &lights, Opt<std::function<String(Map const &, bool, Opt<String>)>> maybeMinimapGenerator={} );
	~Map() noexcept;
	Map()                          = delete;
	Map( Map       && )            = delete;
	Map( Map const  & )            = delete;
	Map& operator=( Map       && ) = delete;
	Map& operator=( Map const  & ) = delete;

	MapConfig const &config;

	V2u                        generateRoadPositionInTileSpace(  RNG & )        const noexcept;
	Vector3                    generateRoadPositionInWorldSpace( RNG & )        const noexcept;
	V2u                        generateNonBuildingPositionInTileSpace(  RNG & ) const noexcept;
	V2u                        generateGroundPositionInTileSpace(  RNG & )      const noexcept;
	Vector3                    generateNonBuildingPositionInWorldSpace( RNG & ) const noexcept;
	Vector3                    generateGroundPositionInWorldSpace( RNG & )      const noexcept;
	V2u                        getStartPositionInTileSpace()                    const noexcept;
	Vector3                    getStartPositionInWorldSpace()                   const noexcept;
	Direction                  getStartDirection()                              const noexcept;
	V2u                        getExitPositionInTileSpace()                     const noexcept;
	Vector3                    getExitPositionInWorldSpace()                    const noexcept;
	TileMap const &            getTileMap()                                     const noexcept;
	Voronoi const &            getDistrictMap()                                 const noexcept;
// TODO: refactor out
	Vector<Opt<V2u>> const &   getHospitalTable() const noexcept;
	Opt<V2u>                   getNearestFoundHospitalTilePos( Vector3 const &sourceWorldPos, UIPlaying const & ) const noexcept;
	Direction                  getHospitalOrientation(   V2u const hospitalTilePos ) const noexcept;
	Vector3                    getHospitalFrontPosition( V2u const hospitalTilePos ) const noexcept;
	District::Enum             districtAt( U32 x, U32 y ) const noexcept;
	inline District::Enum      districtAt( V2u pos )      const noexcept { return districtAt(pos.x, pos.y); }
	District::Enum             districtAt( U32 idx )      const noexcept;
	HouseGenData const &       getHouseData() const noexcept;
	Info const &               getInfo() const noexcept;
	Vector<F32> const &        getRoadDistanceMap() const noexcept;
private:
	void                       eliminateBadSpawnPositions() noexcept;
	void                       placeStreetlight( Vector3 const &worldPosition, Vector3 const &rotation={.0f,.0f,.0f} ) noexcept;
	void                       generateDistricts();
	void                       generateRoads();
	void                       generateBuildings();
	void                       generateBorder();
	void                       generateZebraCrossings();
	void                       generateStreetlights();
	Opt<Lot>                   findRandomLot( U16 districtId ) noexcept;
	Opt<Lot>                   findFixedLot(  U16 districtId, U32 width, U32 length, Vector<bool> const &&layout ) noexcept;
	void                       instantiateTilesAsModels() noexcept;
	MultiTileHouse             instantiateMultitileHouse( V2u const &nw, MultitileLayout &&, HouseTileset const & ) const noexcept;
	CompositeHouse             instantiateSkyscraper();
	Graphics &                 graphics;
	V2u                        startPositionInTileSpace;
	V2u                        exitPositionInTileSpace;
	Direction                  startDirection;
	UPtr<TileMap>              tilemap;
	UPtr<Voronoi>              districtMap;
	Vector<District::Enum>     districtLookupTable;
	Vector<UPtr<GameObject>>   groundTiles;
	Vector<UPtr<GameObject>>   crossingTiles;
	Physics * const            physics;
	LightList &                lights;
	UPtr<Skyscraper>           skyscraperGenerator;
	// TODO: refactor out:
	using DistrictID = U16;
	using BuildingID = U16;                           // 0 = unused tile
	BuildingID                 nextBuildingID { 1U }; // 1 = first valid ID
	Vector<BuildingID>         buildingIDs;
	Vector<F32>                roadDistanceMap;
	BuildingID                 generateBuildingID()      noexcept;
	void                       generateRoadDistanceMap() noexcept;
	Vector<UPtr<Streetlight>>  streetlights;
	Vector<Opt<V2u>>           hospitalTable;
	HouseGenData               houses;
	RNG                        rng;
	Border                     border;
	Info                       info;
	Opt<std::function<void()>> maybeDemoCallback;
};
