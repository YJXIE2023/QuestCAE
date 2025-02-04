// 项目头文件
#include "container/data_value_container.hpp"
#include "container/variables_list_data_value_container.hpp"
#include "container/flags.hpp"
#include "container/variable.hpp"
#include "includes/define_python.hpp"
#include "includes/quest_flags.hpp"
#include "includes/constitutive_law.hpp"
#include "utilities/quaternion.hpp"

namespace Quest{

    namespace py = pybind11;

    Flags FlagsOr(const Flags& Left, const Flags& Right )
    {
        return (Left|Right);
    }

    Flags FlagsAnd(const Flags& Left, const Flags& Right )
    {
        return (Left&Right);
    }

    void FlagsSet1(Flags& ThisFlag, const Flags& OtherFlag )
    {
        ThisFlag.Set(OtherFlag);
    }

    void FlagsSet2(Flags& ThisFlag, const Flags& OtherFlag, bool Value )
    {
        ThisFlag.Set(OtherFlag, Value);
    }

    template<class TContainerType>
    struct Array1DModifier
    {
        typedef typename TContainerType::size_type index_type;
        static void Resize( TContainerType& ThisContainer, typename TContainerType::size_type NewSize ){}

        static void MoveSlice( TContainerType& ThisContainer, index_type Index, index_type From, index_type To ){}
    };


    template< class TBinderType, typename TContainerType, typename TVariableType > 
    void VariableIndexingUtility(TBinderType& binder){
        binder.def("__contains__", [](const TContainerType& container, const TVariableType& rV){return container.Has(rV);} );
        binder.def("__setitem__", [](TContainerType& container, const TVariableType& rV, const typename TVariableType::Type rValue){container.SetValue(rV, rValue);} );
        binder.def("__getitem__", [](TContainerType& container, const TVariableType& rV){return container.GetValue(rV);} );
        binder.def("Has", [](const TContainerType& container, const TVariableType& rV){return container.Has(rV);} );
        binder.def("SetValue",  [](TContainerType& container, const TVariableType& rV, const typename TVariableType::Type rValue){container.SetValue(rV, rValue);} );
        binder.def("GetValue", [](TContainerType& container, const TVariableType& rV){return container.GetValue(rV);} );
        binder.def("Clear", [](TContainerType& container){container.Clear();} );
    }


    template< class TBinderType, typename TContainerType, typename TVariableType > 
    void DataValueContainerIndexingUtility(TBinderType& binder){
        VariableIndexingUtility<TBinderType, TContainerType, TVariableType>(binder);
        binder.def("Erase", [](TContainerType& container, const TVariableType& rV){return container.Erase(rV);} );
    }


    void AddContainersToPython(pybind11::module& m){
        using Array1DVariable3 = Variable<Array1d<double, 3> >;
        using Array1DVariable4 = Variable<Array1d<double, 4> >;
        using Array1DVariable6 = Variable<Array1d<double, 6> >;
        using Array1DVariable9 = Variable<Array1d<double, 9> >;

        py::class_<VariableData>(m, "VariableData")
            .def("Name", &VariableData::Name, py::return_value_policy::copy)
            .def("Key", &VariableData::Key)
            .def("GetSourceVariable", &VariableData::GetSourceVariable)
            .def("GetComponentIndex", &VariableData::GetComponentIndex)
            .def("IsComponent", &VariableData::IsComponent)
            .def("__str__", PrintObject<VariableData>);


        py::class_<Variable<std::string>, VariableData>(m, "StringVariable")
            .def("__str__" , PrintObject<Variable<std::string>>);

        
        py::class_<Variable<bool>, VariableData>(m, "BoolVariable")
            .def("__str__" , PrintObject<Variable<bool>>);

        py::class_<Variable<int>, VariableData>(m, "IntVariable")
            .def("__str__" , PrintObject<Variable<int>>);

        py::class_<Variable<DenseVector<int> >,VariableData>(m, "IntegerVectorVariable")
        .def("__str__", PrintObject<Variable<DenseVector<int> >>);

        py::class_<Variable<double>,VariableData>(m, "DoubleVariable")
        .def("__str__", PrintObject<Variable<double>>);

        py::class_<Variable<Vector >,VariableData>(m, "VectorVariable")
        .def("__str__", PrintObject<Variable<Vector >>);

        py::class_<Array1DVariable3,VariableData>(m, "Array1DVariable3")
        .def("__str__", PrintObject<Array1DVariable3>);

        py::class_<Array1DVariable4,VariableData>(m, "Array1DVariable4")
        .def("__str__", PrintObject<Array1DVariable4>);

        py::class_<Array1DVariable6,VariableData>(m, "Array1DVariable6")
        .def("__str__", PrintObject<Array1DVariable6>);

        py::class_<Array1DVariable9,VariableData>(m, "Array1DVariable9")
        .def("__str__", PrintObject<Array1DVariable9>);

        py::class_<Variable<DenseMatrix<double> >,VariableData>(m, "MatrixVariable")
        .def("__str__", PrintObject<Variable<DenseMatrix<double> >>);

        py::class_<Variable<ConstitutiveLaw::Pointer>,VariableData>(m, "ConstitutuveLawVariable")
        .def("__str__", PrintObject<Variable<ConstitutiveLaw::Pointer>>);

        py::class_<Variable<ConvectionDiffusionSettings::Pointer > ,VariableData>(m,"ConvectionDiffusionSettingsVariable")
        .def("__str__", PrintObject<Variable<ConvectionDiffusionSettings::Pointer >>);

        py::class_<Variable<RadiationSettings::Pointer > ,VariableData>(m,"RadiationSettingsVariable")
        .def("__str__", PrintObject<Variable<RadiationSettings::Pointer >>);

        py::class_<Variable<Quaternion<double> >>(m, "DoubleQuaternionVariable")
        .def("__str__", PrintObject<Variable<Quaternion<double> >>);


        using DataValueContainerBinderType = py::class_<DataValueContainer, DataValueContainer::Pointer>;
        DataValueContainerBinderType DataValueBinder(m, "DataValueContainer");
        DataValueBinder.def("__len__", &DataValueContainer::Size);
        DataValueBinder.def("__str__", PrintObject<DataValueContainer>);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<bool> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<int> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<double> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Array1DVariable3 >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Array1DVariable4 >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Array1DVariable6 >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Array1DVariable9 >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<Vector> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<Matrix> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<ConvectionDiffusionSettings::Pointer> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<RadiationSettings::Pointer> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<Quaternion<double>> >(DataValueBinder);
        DataValueContainerIndexingUtility< DataValueContainerBinderType, DataValueContainer, Variable<std::string> >(DataValueBinder);

        using VariableDataValueContainerBinderType = py::class_<VariablesListDataValueContainer, VariablesListDataValueContainer::Pointer>;
        VariableDataValueContainerBinderType VariableDataValueBinder(m, "VariablesListDataValueContainer");
        VariableDataValueBinder.def("__len__", &VariablesListDataValueContainer::Size);
        VariableDataValueBinder.def("__str__", PrintObject<VariablesListDataValueContainer>);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<bool> >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<int> >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<double> >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Array1DVariable3 >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Array1DVariable4 >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Array1DVariable6 >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Array1DVariable9 >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<Vector> >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<Matrix> >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<Quaternion<double>> >(VariableDataValueBinder);
        VariableIndexingUtility< VariableDataValueContainerBinderType, VariablesListDataValueContainer, Variable<std::string> >(VariableDataValueBinder);


        py::class_<Flags, Flags::Pointer>(m, "Flags")
            .def(py::init<>())
            .def(py::init<Flags>())
            .def("Is", &Flags::Is)
            .def("IsNot", &Flags::IsNot)
            .def("Set", FlagsSet1)
            .def("Set", FlagsSet2)
            .def("IsDefined", &Flags::IsDefined)
            .def("IsNotDefined", &Flags::IsNotDefined)
            .def("Reset", &Flags::Reset)
            .def("Flip", &Flags::Flip)
            .def("Clear", &Flags::Clear)
            .def("AsFalse", &Flags::AsFalse)
            .def("__or__", FlagsOr)
            .def("__and__", FlagsAnd)
            .def("__str__", PrintObject<Flags>);


        QUEST_REGISTER_IN_PYTHON_FLAG(m,STRUCTURE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,INTERFACE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,FLUID);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,INLET);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,OUTLET);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,VISITED);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,THERMAL);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,SELECTED);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,BOUNDARY);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,SLIP);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,CONTACT);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,TO_SPLIT);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,TO_ERASE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,TO_REFINE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,NEW_ENTITY);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,OLD_ENTITY);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,ACTIVE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,MODIFIED);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,RIGID);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,SOLID);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,MPI_BOUNDARY);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,INTERACTION);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,ISOLATED);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,MASTER);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,SLAVE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,INSIDE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,FREE_SURFACE);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,BLOCKED);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,MARKER);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,PERIODIC);
        QUEST_REGISTER_IN_PYTHON_FLAG(m,WALL);


        QUEST_REGISTER_IN_PYTHON_FLAG_IMPLEMENTATION(m,ALL_DEFINED);
        QUEST_REGISTER_IN_PYTHON_FLAG_IMPLEMENTATION(m,ALL_TRUE);

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SPACE_DIMENSION )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DOMAIN_SIZE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, IS_RESTARTED )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, COMPUTE_LUMPED_MASS_MATRIX )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, COMPUTE_DYNAMIC_TANGENT )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, THERMAL_EXPANSION_COEFFICIENT )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, POWER_LAW_N )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, POWER_LAW_K )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, EQ_STRAIN_RATE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, YIELD_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MU )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TAU )


        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONSTRAINT_LABELS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOAD_LABELS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MARKER_LABELS )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONSTRAINT_MESHES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOAD_MESHES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MARKER_MESHES )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ELEMENTAL_DISTANCES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ELEMENTAL_EDGE_DISTANCES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ELEMENTAL_EDGE_DISTANCES_EXTRAPOLATED )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NL_ITERATION_NUMBER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, FRACTIONAL_STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TIME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, START_TIME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, END_TIME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DELTA_TIME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PREVIOUS_DELTA_TIME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INTERVAL_END_TIME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PRINTED_STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PRINTED_RESTART_STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RUNGE_KUTTA_STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RESIDUAL_NORM )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONVERGENCE_RATIO )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, BUILD_SCALE_FACTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONSTRAINT_SCALE_FACTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AUXILIAR_CONSTRAINT_SCALE_FACTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TEMPERATURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TEMPERATURE_OLD_IT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VISCOSITY_AIR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VISCOSITY_WATER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ERROR_RATIO )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TIME_STEPS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PENALTY_COEFFICIENT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SCALAR_LAGRANGE_MULTIPLIER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TIME_INTEGRATION_THETA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SHAPE_FUNCTIONS_VECTOR)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SHAPE_FUNCTIONS_GRADIENT_MATRIX)
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VECTOR_LAGRANGE_MULTIPLIER )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ANGULAR_ACCELERATION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY_LAPLACIAN )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY_LAPLACIAN_RATE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY_COMPONENT_GRADIENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY_X_GRADIENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY_Y_GRADIENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY_Z_GRADIENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ANGULAR_VELOCITY )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ACCELERATION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VELOCITY )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, DISPLACEMENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ROTATION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, DELTA_ROTATION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, REACTION_MOMENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, REACTION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, BODY_FORCE )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, FORCE_RESIDUAL )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, MOMENT_RESIDUAL )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, INTERNAL_FORCE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, EXTERNAL_FORCE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, CONTACT_FORCE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, CONTACT_NORMAL )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, TEMPERATURE_GRADIENT )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, EXTERNAL_FORCES_VECTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INTERNAL_FORCES_VECTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONTACT_FORCES_VECTOR )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, LINEAR_MOMENTUM )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ANGULAR_MOMENTUM )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, VOLUME_ACCELERATION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, SEEPAGE_DRAG )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, NORMAL )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, TANGENT_XI )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, TANGENT_ETA )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, LOCAL_TANGENT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOCAL_TANGENT_MATRIX )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, FORCE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, TORQUE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, MOMENT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, FORCE_CM )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, MOMENTUM_CM )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, MOMENTUM )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, MASS )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, RHS )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, WATER_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, REACTION_WATER_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, WATER_PRESSURE_ACCELERATION )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AIR_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, REACTION_AIR_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RHS_WATER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RHS_AIR )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, WEIGHT_FATHER_NODES )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INTERNAL_ENERGY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, STRAIN_ENERGY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, EXTERNAL_ENERGY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TOTAL_ENERGY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, KINETIC_ENERGY )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VOLUMETRIC_STRAIN )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOCAL_INERTIA_TENSOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOCAL_AXES_MATRIX )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOCAL_CONSTITUTIVE_MATRIX )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONSTITUTIVE_MATRIX )

        // for geometrical application
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, CHARACTERISTIC_GEOMETRY_LENGTH)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DETERMINANTS_OF_JACOBIAN_PARENT)

        //for structural application TO BE REMOVED
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NUMBER_OF_CYCLES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONSTITUTIVE_LAW )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INTERNAL_VARIABLES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MATERIAL_PARAMETERS )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NEGATIVE_FACE_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, POSITIVE_FACE_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, POROSITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DIAMETER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LIN_DARCY_COEF )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NONLIN_DARCY_COEF )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, DRAG_FORCE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, STRUCTURE_VELOCITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, K0 )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NODAL_VOLUME )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, STATIONARY )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, FLAG_VARIABLE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DISTANCE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AUX_DISTANCE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, DISTANCE_GRADIENT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INERTIA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PERIODIC_PAIR_INDEX )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PARTITION_INDEX )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, LAGRANGE_DISPLACEMENT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LAGRANGE_AIR_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LAGRANGE_WATER_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LAGRANGE_TEMPERATURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INTERNAL_FRICTION_ANGLE )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LAGRANGE_DISPLACEMENT )

        // for MultiScale application
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INITIAL_STRAIN )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, COEFFICIENT_THERMAL_EXPANSION )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CHARACTERISTIC_LENGTH_MULTIPLIER )

        //for Incompressible Fluid application
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,FRACT_VEL)

        //for ALE application
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, DETERMINANT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ELEMENTSHAPE )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, MESH_VELOCITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AUX_MESH_VAR )

        //for Adjoint
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, SHAPE_SENSITIVITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NORMAL_SENSITIVITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NUMBER_OF_NEIGHBOUR_ELEMENTS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, UPDATE_SENSITIVITIES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NORMAL_SHAPE_DERIVATIVE )

        //for electric application

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PARTITION_MASK )

        // For MeshingApplication
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NODAL_ERROR )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, NODAL_ERROR_COMPONENTS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ELEMENT_ERROR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ELEMENT_H )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RECOVERED_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ERROR_INTEGRATION_POINT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONTACT_PRESSURE )

        // For explicit time integration
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RESIDUAL_VECTOR )

        //for PFEM application TO BE REMOVED
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NODAL_AREA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NODAL_H )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, NORMAL_TO_WALL )
        //QUEST_REGISTER_IN_PYTHON_VARIABLE(m,NEIGHBOUR_NODES)
        //QUEST_REGISTER_IN_PYTHON_VARIABLE(m,NEIGHBOUR_ELEMENTS)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, FRICTION_COEFFICIENT )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, BULK_MODULUS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SATURATION )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, GRAVITY )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, FACE_LOAD )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DENSITY_WATER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DENSITY_AIR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AIR_ENTRY_VALUE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, FIRST_SATURATION_PARAM )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SECOND_SATURATION_PARAM )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PERMEABILITY_WATER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PERMEABILITY_AIR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, BULK_AIR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SCALE )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TEMP_CONV_PROJ )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CONVECTION_COEFFICIENT)

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INSITU_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, STRESSES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, STRAIN )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NODAL_MASS )
        QUEST_REGISTER_IN_PYTHON_SYMMETRIC_3D_TENSOR_VARIABLE_WITH_COMPONENTS(m, NODAL_INERTIA_TENSOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AUX_INDEX )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, EXTERNAL_PRESSURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, BDF_COEFFICIENTS )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, ROTATION_CENTER )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VELOCITY_PERIOD )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ANGULAR_VELOCITY_PERIOD )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, IDENTIFIER )


        //for xfem application
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CRACK_OPENING )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CRACK_TRANSLATION )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,ARRHENIUS)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,ARRHENIUSAUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,ARRHENIUSAUX_)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,PRESSUREAUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,NODAL_MAUX)
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,NODAL_VAUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,NODAL_PAUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,FACE_HEAT_FLUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,HEAT_FLUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,REACTION_FLUX)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,TC)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,CONDUCTIVITY)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,SPECIFIC_HEAT)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,MATERIAL_VARIABLE)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,FUEL)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YO)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YF)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YI)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,Y1)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,Y2)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YP)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,EMISSIVITY)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,ENTHALPY)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,MIXTURE_FRACTION)

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YCH4)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YO2)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YCO2)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YH2O)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,YN2)

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,INCIDENT_RADIATION_FUNCTION)

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,ABSORPTION_COEFFICIENT)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,STEFAN_BOLTZMANN_CONSTANT)
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, DIRECTION )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m,NODAL_SWITCH)
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,Y)

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, LOCAL_AXIS_1 )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, LOCAL_AXIS_2 )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, LOCAL_AXIS_3 )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SWITCH_TEMPERATURE )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,EMBEDDED_VELOCITY)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, REFINEMENT_LEVEL )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AIR_SOUND_VELOCITY )

        // for Vulcan application
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LATENT_HEAT )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ENRICHED_PRESSURES )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INITIAL_PENALTY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DP_EPSILON )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DP_ALPHA1 )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, DP_K )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LAMBDA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MIN_DT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MAX_DT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, WET_VOLUME)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, CUTTED_AREA)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NET_INPUT_MATERIAL)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, MIU )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SCALE_FACTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NORMAL_CONTACT_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TANGENTIAL_CONTACT_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_FACTOR )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NEWMARK_BETA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NEWMARK_GAMMA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, BOSSAK_ALPHA )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, EQUILIBRIUM_POINT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AIR_SOUND_VELOCITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, WATER_SOUND_VELOCITY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ACTIVATION_LEVEL )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, FIRST_TIME_STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, QUASI_STATIC_ANALYSIS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, FRACTIONAL_STEP )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LOAD_RESTART )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, RIGID_BODY_ID )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, NORMAL_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, TANGENTIAL_STRESS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PENALTY )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, AMBIENT_TEMPERATURE )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VEL_ART_VISC )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, PR_ART_VISC )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, LIMITER_COEFFICIENT )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SOUND_VELOCITY )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, SEARCH_RADIUS )
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, ORIENTATION )

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, INTEGRATION_WEIGHT )
        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, INTEGRATION_COORDINATES )

        QUEST_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m, PARAMETER_2D_COORDINATES)

        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VARIATIONAL_REDISTANCE_COEFFICIENT_FIRST)
        QUEST_REGISTER_IN_PYTHON_VARIABLE(m, VARIATIONAL_REDISTANCE_COEFFICIENT_SECOND)

    }

}