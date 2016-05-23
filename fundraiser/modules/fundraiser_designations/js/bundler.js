(function ($) {
    Drupal.behaviors.fdBundler = {
        attach: function (context, settings) {
            $(window).ready(function () {
                window.App = {
                    Handlers: {}
                };
                App.Handlers.reorderItems = function () {

                    $('#fundraiser-designations-categories table:not("table.sticky-header")').draggable({
                        connectToSortable: "#fundraiser-designations-groups form",
                        helper: "clone",
                        revert: false,
                        placeholder: 'sortable-placeholder',
                        opacity: 0.5,
                        delay: 150,
                        cursorAt: {left: 5},
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        }
                    });

                    $(".designations-category-table tr").draggable({
                        connectToSortable: "#fundraiser-designations-groups form table",
                        helper: "clone",
                        revert: false,
                        cancel: 'th',
                        delay: 150,
                        placeholder: 'sortable-placeholder',
                        cursorAt: {left: 5},
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        }
                    });

                    $('#fundraiser-designations-groups form table:not("table.sticky-header") tr').draggable({
                        connectToSortable: "#fundraiser-designations-groups table",
                        helper: "original",
                        revert: false,
                        cancel: 'th',
                        placeholder: 'sortable-placeholder',
                        cursorAt: {left: 5}
                    });


                    $('#fundraiser-designations-groups form table:not("table.sticky-header")').sortable({
                        cancel: 'th, thead',
                        placeholder: 'sortable-placeholder',
                        items: 'tr:not("thead tr")',
                        cursorAt: {left: 5},
                        forcePlaceholderSize: true,
                        opacity: 0.5,
                        delay: 150,
                        start: function (event, ui) {
                        },
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        },
                    });

                    $('#fundraiser-designations-groups form').sortable({
                        placeholder: 'sortable-placeholder',
                        items: 'table:not("table.sticky-header")',
                        cursorAt: {left: 5},
                        tolerance: "pointer",
                        opacity: 0.5,
                        delay: 150,
                        //forcePlaceholderSize: true,
                        start: function (event, ui) {
                        },
                        stop: function (event, ui) {
                            App.Handlers.reorderItems();
                        },
                    });
                }

                App.Handlers.reorderItems();
            }); // End window.ready
        }
    }
})(jQuery);
