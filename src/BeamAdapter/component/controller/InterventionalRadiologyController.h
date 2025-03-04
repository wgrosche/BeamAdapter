/******************************************************************************
*                              BeamAdapter plugin                             *
*                  (c) 2006 Inria, University of Lille, CNRS                  *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: see Authors.md                                                     *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
//
// C++ Implementation : InterventionalRadiologyController
//
// Description:
//
//
// Author: Christian Duriez, INRIA
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
#pragma once

#include <sofa/component/controller/MechanicalStateController.h>
#include <sofa/component/topology/container/dynamic/EdgeSetTopologyModifier.h>
#include <sofa/defaulttype/SolidTypes.h>
#include <sofa/component/constraint/projective/FixedConstraint.h>
#include <sofa/core/DataEngine.h>
#include <sofa/component/collision/geometry/PointModel.h>
#include <sofa/component/collision/geometry/LineModel.h>

#include <BeamAdapter/utils/BeamActions.h>
#include <BeamAdapter/component/WireBeamInterpolation.h>
#include <sofa/component/topology/container/dynamic/EdgeSetGeometryAlgorithms.h>
#include <sofa/component/topology/container/dynamic/EdgeSetTopologyModifier.h>

namespace sofa::component::controller
{

namespace _interventionalradiologycontroller_
{

using sofa::type::Vec;
using sofa::type::Vec3d;
using sofa::core::topology::BaseMeshTopology;
using sofa::component::constraint::projective::FixedConstraint;

/*!
 * \class InterventionalRadiologyController
 * @brief InterventionalRadiologyController Class
 *
 * Provides a Mouse & Keyboard user control on an EdgeSet Topology.
 */
template<class DataTypes>
class InterventionalRadiologyController : public MechanicalStateController<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(InterventionalRadiologyController,DataTypes),SOFA_TEMPLATE(MechanicalStateController,DataTypes));
    typedef typename DataTypes::VecCoord                            VecCoord;
    typedef typename DataTypes::VecDeriv                            VecDeriv;
    typedef typename DataTypes::Coord                               Coord   ;
    typedef typename DataTypes::Deriv                               Deriv   ;
    typedef typename Coord::value_type                              Real    ;
    typedef typename defaulttype::SolidTypes<Real>::Transform       Transform;
    typedef typename defaulttype::SolidTypes<Real>::SpatialVector   SpatialVector;
    typedef typename std::vector<Real>::const_iterator              RealConstIterator;

    typedef Vec<3, Real>                            Vec3;
    typedef BaseMeshTopology::EdgeID                ElementID;
    typedef type::vector<BaseMeshTopology::EdgeID>        VecElementID;
    typedef MechanicalStateController<DataTypes>    Inherit;
    typedef fem::WireBeamInterpolation<DataTypes>   WBeamInterpolation;

public:
    InterventionalRadiologyController();
    virtual ~InterventionalRadiologyController() = default;

    ////////////////////// Inherited from BaseObject ///////////////////////////////////////////////
    virtual void init() override ;
    virtual void bwdInit() override ;
    ////////////////////////////////////////////////////////////////////////////////////////////////


    ////////////////////// Inherited from Controller ///////////////////////////////////////////////
    virtual void onMouseEvent(core::objectmodel::MouseEvent *) override ;
    virtual void onKeyPressedEvent(core::objectmodel::KeypressedEvent *) override ;
    virtual void onBeginAnimationStep(const double dt) override ;
    ////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////

    virtual bool modifyTopology(void);
    void interventionalRadiologyCollisionControls(type::vector<Real> &x_point_list,
                                                  type::vector<int> &id_instrument_list,
                                                  type::vector<int> &removeEdge);
    void getInstrumentList(type::vector<sofa::component::fem::WireBeamInterpolation<DataTypes>*>& list);
    const type::vector< type::vector<int> >& get_id_instrument_curvAbs_table()const;
    int getTotalNbEdges()const;

    void applyAction(sofa::beamadapter::BeamAdapterAction action);

    /// Getter to the tools curviline abscisses sorted @sa m_nodeCurvAbs at the current timestep.
    [[nodiscard]] const type::vector<Real>& getCurrentCurvAbscisses() const { return m_nodeCurvAbs; }

public:

    using Inherit1::f_printLog;
    using Inherit1::f_listening;
    using Inherit1::getContext;
    using Inherit1::getMechanicalState;

    /// Conditional elements for construction of InterventionalRadiologyController
    Data< type::vector< std::string > >  d_instrumentsPath;
    type::vector< WBeamInterpolation * > m_instrumentsList;

    /// For point and line activer
    type::vector<bool> m_activatedPointsBuf;

    /// Interface for interventionalRadiology instruments:
    virtual void applyInterventionalRadiologyController(void);

    void processDrop(unsigned int &previousNumControlledNodes,  unsigned int &seg_remove);
    void interventionalRadiologyComputeSampling(type::vector<Real> &newCurvAbs, type::vector< type::vector<int> > &id_instrument_table, const type::vector<Real> &xBegin, const Real& xEnd);
    /// Sort the curv Abs in the ascending order and avoid doubloon
    void sortCurvAbs(type::vector<Real> &CurvAbs, type::vector< type::vector<int> >& id_instrument_table);
    void totalLengthIsChanging(const type::vector<Real>& newNodeCurvAbs, type::vector<Real>& modifiedNodeCurvAbs, const type::vector< type::vector<int> >& newTable);
    void fixFirstNodesWithUntil(unsigned int first_simulated_Node);
    void activateBeamListForCollision( type::vector<Real> &curv_abs, type::vector< type::vector<int> > &id_instrument_table);
    void loadMotionData(std::string filename);

    Data<int>            d_controlledInstrument;
    Data<type::vector<Real>>   d_xTip;
    Data<type::vector<Real>>   d_rotationInstrument;
    Data<Real>           d_step;
    Data<Real>           d_angularStep;
    Data<Real>           d_speed;
    Data<Coord>          d_startingPos;
    Data<Real>           d_threshold;
    Data<Real>           d_instrumentSpireDiameter;
    Data<Real>           d_bendStep;
    Data<type::vector<Real>>   d_rigidCurvAbs; // Pairs (start - end)
    Data<std::string>    d_motionFilename;
    Data<unsigned int>   d_indexFirstNode; // First Node simulated
    
    

    bool m_FF, m_RW, m_sensored;
    FixedConstraint<DataTypes> *    m_fixedConstraint;
    type::vector<int>                     m_droppedInstruments;
    type::vector<Vec3d>                   m_sensorMotionData;
    unsigned int                    m_currentSensorData;
    type::vector<Real>                    m_nodeCurvAbs;
    type::vector< type::vector<int> >           m_idInstrumentCurvAbsTable;
    unsigned int                    m_numControlledNodes; // Excluding the nodes that are "dropped"
    bool                            m_dropCall;
};

#if !defined(SOFA_PLUGIN_BEAMADAPTER_INTERVENTIONALRADIOCONTROLLER_CPP)
extern template class SOFA_BEAMADAPTER_API InterventionalRadiologyController<sofa::defaulttype::Rigid3Types>;
#endif

} // namespace _interventionalradiologycontroller_

using _interventionalradiologycontroller_::InterventionalRadiologyController;

} // namespace sofa::component::controller
