/*
 * A visual to label Cyclic colour maps
 */

#pragma once

#include <deque>

#include <sm/mathconst>
#include <sm/vec>

#include <mplot/VisualModel.h>
#include <mplot/unicode.h>

namespace mplot {

    template <typename F, int glver = mplot::gl::version_4_1>
    class CyclicColourVisual : public VisualModel<glver>
    {
        using mc = sm::mathconst<F>;

    public:
        //! Constructor
        //! \param _offset The offset within mplot::Visual space to place this model
        CyclicColourVisual (const sm::vec<float> _offset)
        {
            this->mv_offset = _offset;
            this->viewmatrix.translate (this->mv_offset);
            // Set default text features
            this->tf.fontsize = 0.15f;
            this->tf.fontres = 36;
            this->tf.colour = this->framecolour;
            this->twodimensional = true;
            this->labels.clear();
            this->labels = { unicode::toUtf8(unicode::pi)+std::string("/2"), unicode::toUtf8(unicode::pi),
                             std::string("3")+unicode::toUtf8(unicode::pi)+std::string("/2"), "0" };
        }

        void setTextColour (const std::array<float, 3>& c) { this->tf.colour = c; }
        void setFrameColour (const std::array<float, 3>& c) { this->framecolour = c; }
        void setColour (const std::array<float, 3>& c)
        {
            this->tf.colour = c;
            this->framecolour = c;
        }

        void initializeVertices()
        {
            // Auto-set ticklabelgap
            auto em = this->makeVisualTextModel (tf);
            mplot::TextGeometry em_geom = em->getTextGeometry (std::string("m"));
            this->ticklabelgap = em_geom.width()/2.0f;
            if (this->draw_frame == true) { this->drawFrame(); }
            if (this->draw_ticks == true) { this->drawTickLabels(); }
            this->fillFrameWithColour();
        }

        //! Draw a circular frame around the outside and one around the inside
        void drawFrame()
        {
            // Draw an approximation of a circle.
            this->computeFlatCircleLine (sm::vec<float>({0,0,this->z}), this->uz, this->outer_radius + this->framelinewidth/2.0f,
                                         this->framelinewidth, this->framecolour, this->numsegs);

            this->computeFlatCircleLine (sm::vec<float>({0,0,this->z}), this->uz, this->inner_radius + this->framelinewidth/2.0f,
                                         this->framelinewidth, this->framecolour, this->numsegs);
        }

        //! Draw the tick labels (the numbers or whatever text the client code has given us)
        void drawTickLabels()
        {
            // Reset these members
            this->ticklabelheight = 0.0f;
            this->ticklabelwidth = 0.0f;

            if (this->label_angles.empty()) {
                // Auto-fill label_angles based on labels size.
                // example order for 4: mc::pi_over_2, mc::pi, mc::three_pi_over_2, 0.0f
                this->label_angles.resize (this->labels.size());
                for (unsigned int i = 0; i < this->labels.size(); ++i) {
                    // North is pi/2, so that's the start:
                    this->label_angles[i] = mc::pi_over_2 + i * (mc::two_pi / this->labels.size());
                    // Rescale any that exceed 2pi:
                    this->label_angles[i] = this->label_angles[i] < F{0} ? this->label_angles[i] + mc::two_pi : this->label_angles[i];
                    this->label_angles[i] = this->label_angles[i] > mc::two_pi ? this->label_angles[i] - mc::two_pi : this->label_angles[i];
                }
            }

            for (unsigned int i = 0; i < this->label_angles.size(); ++i) {
                std::string s = this->labels[i];
                auto lbl = this->makeVisualTextModel (this->tf);
                mplot::TextGeometry geom = lbl->getTextGeometry (s);
                this->ticklabelheight = geom.height() > this->ticklabelheight ? geom.height() : this->ticklabelheight;
                this->ticklabelwidth = geom.width() > this->ticklabelwidth ? geom.width() : this->ticklabelwidth;
                // Dep. on angle, the additional gap for the text will need to be based on different aspects of the text geometry
                float geom_gap = std::abs(std::cos(label_angles[i]) * geom.half_width()) + std::abs(std::sin(label_angles[i]) * geom.half_height());
                float lbl_r = this->outer_radius + this->framelinewidth + this->ticklabelgap + geom_gap;
                sm::vec<float> lblpos = {
                    lbl_r * std::cos (label_angles[i]) - geom.half_width(),
                    lbl_r * std::sin (label_angles[i]) - geom.half_height(),
                    this->z
                };
                lbl->setupText (s, lblpos+this->mv_offset, this->tf.colour);
                this->texts.push_back (std::move(lbl));
            }
        }

        void fillFrameWithColour()
        {
            sm::vec<float> centre = {0,0,this->z};

            for (int ring = this->numrings; ring > 0; ring--) {

                float r_d = this->outer_radius - this->inner_radius;
                float r_dr = r_d / this->numrings;

                float r_out = this->inner_radius + r_dr * static_cast<float>(ring);
                float r_in = this->inner_radius + r_dr * static_cast<float>(ring-1);

                float norm_r_out = (r_out - this->inner_radius) / r_d; // range 0->1
                float norm_r_in = (r_in - this->inner_radius) / r_d;

                for (int j = 0; j < static_cast<int>(this->numsegs); j++) {

                    // The colour will not change for each j
                    float colour_angle = (static_cast<float>(j)/this->numsegs) * sm::mathconst<float>::two_pi;
                    float decaying_sine_out = 0.0f;
                    float decaying_sine_in = 0.0f;
                    if (this->show_perception_sine) {
                        decaying_sine_out = 0.1f * norm_r_out * norm_r_out * std::sin (20.0f * mc::pi * colour_angle);
                        decaying_sine_in = 0.1f * norm_r_in * norm_r_in * std::sin (20.0f * mc::pi * colour_angle);
                    }
                    std::array<float, 3> col_out = this->cm.convert (colour_angle/mc::two_pi + decaying_sine_out);
                    std::array<float, 3> col_in = this->cm.convert (colour_angle/mc::two_pi + decaying_sine_in);

                    float t = j * sm::mathconst<float>::two_pi/static_cast<float>(this->numsegs);
                    sm::vec<float> c_in = this->uy * sin(t) * r_in + this->ux * cos(t) * r_in;
                    this->vertex_push (centre+c_in, this->vertexPositions);
                    this->vertex_push (this->uz, this->vertexNormals);
                    this->vertex_push (col_in, this->vertexColors);
                    sm::vec<float> c_out = this->uy * sin(t) * r_out + this->ux * cos(t) * r_out;
                    this->vertex_push (centre+c_out, this->vertexPositions);
                    this->vertex_push (this->uz, this->vertexNormals);
                    this->vertex_push (col_out, this->vertexColors);
                }
                // Added 2*segments vertices to vertexPositions

                for (int j = 0; j < static_cast<int>(this->numsegs); j++) {
                    int jn = (numsegs + ((j+1) % numsegs)) % numsegs;
                    this->indices.push_back (this->idx+(2*j));
                    this->indices.push_back (this->idx+(2*jn));
                    this->indices.push_back (this->idx+(2*jn+1));
                    this->indices.push_back (this->idx+(2*j));
                    this->indices.push_back (this->idx+(2*jn+1));
                    this->indices.push_back (this->idx+(2*j+1));
                }
                this->idx += 2 * this->numsegs; // nverts
            }
        }

        //! The ColourMap to show (copy in). Should be a cyclic ColourMap
        mplot::ColourMap<F> cm;
        // Show a perceptual test sine?
        bool show_perception_sine = true;
        //! The radii of the cyclic disc
        float outer_radius = 1.0f;
        float inner_radius = 0.3f;
        //! Position in z in model space. Default is just 0.
        float z = 0.0f;
        //! colour for the axis box/lines. Text colour is in TextFeatures tf.colour
        std::array<float, 3> framecolour = mplot::colour::black;
        bool draw_frame = false;
        //! The line width of the frame
        float framelinewidth = 0.01f;
        //! The label strings  that should be displayed. Order the elements anti-clockwise, starting from the 'north' element.
        std::deque<std::string> labels = { "pi/2", "pi", "3pi/2", "0"};
        //! The positions, as angles for the labels. If empty, these will be auto-computed
        std::deque<F> label_angles = { /*mc::pi_over_2, mc::pi, mc::three_pi_over_2, 0.0f*/ };
        // Stores all the text features for this ColourBar (font, colour, font size, font res)
        mplot::TextFeatures tf;
        //! Option to show/hide tick labels
        bool draw_ticks = true;
        //! Gap to x axis tick labels. Gets auto-set
        float ticklabelgap = 0.05f;
        //! The number of segments to make in each ring of the colourmap fill
        unsigned int numsegs = 128;
        //! How many rings of colour?
        unsigned int numrings = 64;
    protected:
        //! tick label height
        float ticklabelheight = 0.0f;
        //! tick label width
        float ticklabelwidth = 0.0f;
    };

} // namespace
