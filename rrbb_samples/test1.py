#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# GNU Radio version: 3.10.10.0

from PyQt5 import Qt
from gnuradio import qtgui
from PyQt5 import QtCore
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import soapy
import sip



class test1(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Not titled yet", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Not titled yet")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except BaseException as exc:
            print(f"Qt GUI: Could not set Icon: {str(exc)}", file=sys.stderr)
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "test1")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 2560000
        self.rf_gain = rf_gain = 20
        self.gain_values = gain_values = (0.0, 0.9, 1.4, 2.7, 3.7, 7.7, 8.7, 12.5, 14.4, 15.7, 16.6, 19.7, 20.7, 22.9, 25.4, 28.0, 29.7, 32.8, 33.8, 36.4, 37.2, 38.6, 40.2, 42.1, 43.4, 43.9, 44.5, 48.0, 49.6)
        self.freq_offset = freq_offset = 3800
        self.decim_rate = decim_rate = 32
        self.auto_gain = auto_gain = 0

        ##################################################
        # Blocks
        ##################################################

        self._rf_gain_range = qtgui.Range(0, (len(gain_values)-1), 1, 20, 200)
        self._rf_gain_win = qtgui.RangeWidget(self._rf_gain_range, self.set_rf_gain, "'rf_gain'", "counter_slider", int, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._rf_gain_win)
        self._auto_gain_choices = {'Pressed': bool(1), 'Released': bool(0)}

        _auto_gain_toggle_switch = qtgui.GrToggleSwitch(self.set_auto_gain, '', self._auto_gain_choices, False, "green", "gray", 4, 50, 1, 1, self, 'value')
        self.auto_gain = _auto_gain_toggle_switch

        self.top_layout.addWidget(_auto_gain_toggle_switch)
        self.soapy_rtlsdr_source_0 = None
        dev = 'driver=rtlsdr'
        stream_args = 'bufflen=16384'
        tune_args = ['']
        settings = ['']

        def _set_soapy_rtlsdr_source_0_gain_mode(channel, agc):
            self.soapy_rtlsdr_source_0.set_gain_mode(channel, agc)
            if not agc:
                  self.soapy_rtlsdr_source_0.set_gain(channel, self._soapy_rtlsdr_source_0_gain_value)
        self.set_soapy_rtlsdr_source_0_gain_mode = _set_soapy_rtlsdr_source_0_gain_mode

        def _set_soapy_rtlsdr_source_0_gain(channel, name, gain):
            self._soapy_rtlsdr_source_0_gain_value = gain
            if not self.soapy_rtlsdr_source_0.get_gain_mode(channel):
                self.soapy_rtlsdr_source_0.set_gain(channel, gain)
        self.set_soapy_rtlsdr_source_0_gain = _set_soapy_rtlsdr_source_0_gain

        def _set_soapy_rtlsdr_source_0_bias(bias):
            if 'biastee' in self._soapy_rtlsdr_source_0_setting_keys:
                self.soapy_rtlsdr_source_0.write_setting('biastee', bias)
        self.set_soapy_rtlsdr_source_0_bias = _set_soapy_rtlsdr_source_0_bias

        self.soapy_rtlsdr_source_0 = soapy.source(dev, "sc8", 1, '',
                                  stream_args, tune_args, settings)

        self._soapy_rtlsdr_source_0_setting_keys = [a.key for a in self.soapy_rtlsdr_source_0.get_setting_info()]

        self.soapy_rtlsdr_source_0.set_sample_rate(0, samp_rate)
        self.soapy_rtlsdr_source_0.set_frequency(0, 391250000)
        self.soapy_rtlsdr_source_0.set_frequency_correction(0, 0)
        self.set_soapy_rtlsdr_source_0_bias(bool(False))
        self._soapy_rtlsdr_source_0_gain_value = gain_values[rf_gain]
        self.set_soapy_rtlsdr_source_0_gain_mode(0, bool(auto_gain))
        self.set_soapy_rtlsdr_source_0_gain(0, 'TUNER', gain_values[rf_gain])
        self.qtgui_histogram_sink_x_0_0_0 = qtgui.histogram_sink_f(
            16384,
            256,
            (-128),
            127,
            "Mixed IQ",
            1,
            None # parent
        )

        self.qtgui_histogram_sink_x_0_0_0.set_update_time(0.10)
        self.qtgui_histogram_sink_x_0_0_0.enable_autoscale(True)
        self.qtgui_histogram_sink_x_0_0_0.enable_accumulate(False)
        self.qtgui_histogram_sink_x_0_0_0.enable_grid(False)
        self.qtgui_histogram_sink_x_0_0_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers= [-1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_histogram_sink_x_0_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_histogram_sink_x_0_0_0.set_line_label(i, labels[i])
            self.qtgui_histogram_sink_x_0_0_0.set_line_width(i, widths[i])
            self.qtgui_histogram_sink_x_0_0_0.set_line_color(i, colors[i])
            self.qtgui_histogram_sink_x_0_0_0.set_line_style(i, styles[i])
            self.qtgui_histogram_sink_x_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_histogram_sink_x_0_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_histogram_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_histogram_sink_x_0_0_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_histogram_sink_x_0_0_0_win)
        self.qtgui_histogram_sink_x_0_0 = qtgui.histogram_sink_f(
            (16384//2),
            256,
            (-128),
            127,
            "Q only",
            1,
            None # parent
        )

        self.qtgui_histogram_sink_x_0_0.set_update_time(0.10)
        self.qtgui_histogram_sink_x_0_0.enable_autoscale(True)
        self.qtgui_histogram_sink_x_0_0.enable_accumulate(False)
        self.qtgui_histogram_sink_x_0_0.enable_grid(False)
        self.qtgui_histogram_sink_x_0_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers= [-1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_histogram_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_histogram_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_histogram_sink_x_0_0.set_line_width(i, widths[i])
            self.qtgui_histogram_sink_x_0_0.set_line_color(i, colors[i])
            self.qtgui_histogram_sink_x_0_0.set_line_style(i, styles[i])
            self.qtgui_histogram_sink_x_0_0.set_line_marker(i, markers[i])
            self.qtgui_histogram_sink_x_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_histogram_sink_x_0_0_win = sip.wrapinstance(self.qtgui_histogram_sink_x_0_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_histogram_sink_x_0_0_win)
        self.qtgui_histogram_sink_x_0 = qtgui.histogram_sink_f(
            (16384//2),
            256,
            (-128),
            128,
            "I only",
            1,
            None # parent
        )

        self.qtgui_histogram_sink_x_0.set_update_time(0.10)
        self.qtgui_histogram_sink_x_0.enable_autoscale(True)
        self.qtgui_histogram_sink_x_0.enable_accumulate(False)
        self.qtgui_histogram_sink_x_0.enable_grid(False)
        self.qtgui_histogram_sink_x_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers= [-1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_histogram_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_histogram_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_histogram_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_histogram_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_histogram_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_histogram_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_histogram_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_histogram_sink_x_0_win = sip.wrapinstance(self.qtgui_histogram_sink_x_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_histogram_sink_x_0_win)
        self._freq_offset_range = qtgui.Range(-4000, 4000, 100, 3800, 200)
        self._freq_offset_win = qtgui.RangeWidget(self._freq_offset_range, self.set_freq_offset, "'freq_offset'", "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._freq_offset_win)
        self.blocks_vector_to_stream_0 = blocks.vector_to_stream(gr.sizeof_char*1, 2)
        self.blocks_skiphead_0 = blocks.skiphead(gr.sizeof_char*1, (2560000 * 2))
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_char*1)
        self.blocks_head_0 = blocks.head(gr.sizeof_char*1, (16384*160*30))
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, '/home/ctn008/RfSurvey/rrbb_samples/s2m56_gidx20_iq03.int8', False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.blocks_deinterleave_0 = blocks.deinterleave(gr.sizeof_char*1, 1)
        self.blocks_char_to_float_0_1 = blocks.char_to_float(1, 1)
        self.blocks_char_to_float_0_0 = blocks.char_to_float(1, 1)
        self.blocks_char_to_float_0 = blocks.char_to_float(1, 1)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_char_to_float_0, 0), (self.qtgui_histogram_sink_x_0, 0))
        self.connect((self.blocks_char_to_float_0_0, 0), (self.qtgui_histogram_sink_x_0_0, 0))
        self.connect((self.blocks_char_to_float_0_1, 0), (self.qtgui_histogram_sink_x_0_0_0, 0))
        self.connect((self.blocks_deinterleave_0, 0), (self.blocks_char_to_float_0, 0))
        self.connect((self.blocks_deinterleave_0, 1), (self.blocks_char_to_float_0_0, 0))
        self.connect((self.blocks_head_0, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.blocks_head_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.blocks_skiphead_0, 0), (self.blocks_head_0, 0))
        self.connect((self.blocks_vector_to_stream_0, 0), (self.blocks_char_to_float_0_1, 0))
        self.connect((self.blocks_vector_to_stream_0, 0), (self.blocks_deinterleave_0, 0))
        self.connect((self.blocks_vector_to_stream_0, 0), (self.blocks_skiphead_0, 0))
        self.connect((self.soapy_rtlsdr_source_0, 0), (self.blocks_vector_to_stream_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "test1")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.soapy_rtlsdr_source_0.set_sample_rate(0, self.samp_rate)

    def get_rf_gain(self):
        return self.rf_gain

    def set_rf_gain(self, rf_gain):
        self.rf_gain = rf_gain
        self.set_soapy_rtlsdr_source_0_gain(0, 'TUNER', self.gain_values[self.rf_gain])

    def get_gain_values(self):
        return self.gain_values

    def set_gain_values(self, gain_values):
        self.gain_values = gain_values
        self.set_soapy_rtlsdr_source_0_gain(0, 'TUNER', self.gain_values[self.rf_gain])

    def get_freq_offset(self):
        return self.freq_offset

    def set_freq_offset(self, freq_offset):
        self.freq_offset = freq_offset

    def get_decim_rate(self):
        return self.decim_rate

    def set_decim_rate(self, decim_rate):
        self.decim_rate = decim_rate

    def get_auto_gain(self):
        return self.auto_gain

    def set_auto_gain(self, auto_gain):
        self.auto_gain = auto_gain
        self.set_soapy_rtlsdr_source_0_gain_mode(0, self.auto_gain)




def main(top_block_cls=test1, options=None):

    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
