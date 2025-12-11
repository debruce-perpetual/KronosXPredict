#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>

#include <nlohmann/json.hpp>

#include "KronosXPredict/api.hpp"
#include "KronosXPredict/runtime.hpp"
#include "KronosXPredict/plugin_loader.hpp"

namespace py = pybind11;
using json = nlohmann::json;

namespace KronosXPredict {

class PyRealtimeWrapper {
public:
    PyRealtimeWrapper(std::shared_ptr<PluginLibrary> lib,
                      std::unique_ptr<IRealtimeModel, RealtimeDestroyFn> model)
        : lib_(std::move(lib)), model_(std::move(model)) {}

    void ingest(py::array_t<Real> endogenous,
                py::array_t<Real> exogenous,
                std::chrono::steady_clock::time_point t) {
        auto e = endogenous.unchecked<1>();
        auto x = exogenous.unchecked<1>();

        std::vector<Real> e_vec(e.size());
        std::vector<Real> x_vec(x.size());
        for (ssize_t i = 0; i < e.size(); ++i) e_vec[i] = e(i);
        for (ssize_t i = 0; i < x.size(); ++i) x_vec[i] = x(i);

        Observation obs{
            t,
            std::span<const Real>(e_vec.data(), e_vec.size()),
            std::span<const Real>(x_vec.data(), x_vec.size())
        };

        // ingest is expected to copy anything it needs from the spans.
        model_->ingest(obs);
    }

    bool ready() const {
        return model_->ready();
    }

    py::dict predict(TargetKind kind, int steps_ahead, bool want_uncertainty) const {
        PredictionRequest req;
        req.target_kind      = kind;
        req.steps_ahead      = steps_ahead;
        req.want_uncertainty = want_uncertainty;

        PredictionResult r = model_->predict(req);

        py::dict out;
        out["steps_ahead"] = r.steps_ahead;
        out["target_kind"] = static_cast<int>(r.target_kind);
        out["mean"]        = r.mean;
        if (r.variance) {
            out["variance"] = *r.variance;
        }
        out["scalars"] = r.scalars;
        return out;
    }

private:
    std::shared_ptr<PluginLibrary> lib_;
    std::unique_ptr<IRealtimeModel, RealtimeDestroyFn> model_;
};

} // namespace KronosXPredict

PYBIND11_MODULE(kronospredict, m) {
    using namespace KronosXPredict;

    py::enum_<TargetKind>(m, "TargetKind")
        .value("Price", TargetKind::Price)
        .value("Return", TargetKind::Return)
        .value("Volatility", TargetKind::Volatility)
        .value("Direction", TargetKind::Direction)
        .value("EventIntensity", TargetKind::EventIntensity)
        .value("Custom", TargetKind::Custom);

    py::class_<PyRealtimeWrapper>(m, "RealtimeModel")
        .def("ingest", &PyRealtimeWrapper::ingest,
             py::arg("endogenous"),
             py::arg("exogenous"),
             py::arg("t"))
        .def_property_readonly("ready", &PyRealtimeWrapper::ready)
        .def("predict", &PyRealtimeWrapper::predict,
             py::arg("target_kind"),
             py::arg("steps_ahead") = 1,
             py::arg("want_uncertainty") = true);

    m.def(
        "load_model",
        [](const std::string& plugin_path, const py::dict& config_dict) {
            // Convert Python dict -> JSON string using Python's json.dumps,
            // then parse with nlohmann::json.
            py::object json_mod = py::module_::import("json");
            py::object dumps = json_mod.attr("dumps");
            py::object s_obj = dumps(config_dict);
            std::string s = s_obj.cast<std::string>();
            json cfg = json::parse(s);

            auto lib = load_plugin_library(plugin_path);
            auto model = lib->create_realtime(cfg);
            return PyRealtimeWrapper(lib, std::move(model));
        },
        py::arg("plugin_path"),
        py::arg("config"));

    m.def("torch_demo", []() {
        auto r = KronosXPredict::torch_demo();
        return py::array_t<KronosXPredict::Real>(
            {static_cast<py::ssize_t>(r.rows),
             static_cast<py::ssize_t>(r.cols)},
            {static_cast<py::ssize_t>(r.cols * sizeof(KronosXPredict::Real)),
             static_cast<py::ssize_t>(sizeof(KronosXPredict::Real))},
            r.data.data()
        );
    });
}
